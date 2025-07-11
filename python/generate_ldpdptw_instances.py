import json
import numpy as np
import os
import random

# Constants for the generation
HORIZON = 14400  # 4h
TIME_STEP = 600  # 10min
DURATIONS = [1800, 5400]  # 30min, 1h30
SERVICE_DURATION = 180 # 3min
VEHICLE_CAPACITY = 300


# Generate an array of delivery location IDs
def generate_delivery(rng, matrix_size, instance_size):
    return rng.choice(range(matrix_size), size=instance_size, replace=True)

# Generate an array of pickup location IDs with a pickup/delivery ratio
def generate_pickups(rng, matrix_size, instance_size):
    PICKUP_DELIVERY_RATIO = 1 / 10
    pickup_count = np.ceil(instance_size * PICKUP_DELIVERY_RATIO)
    choosen_pickups = rng.choice(range(matrix_size), size=int(pickup_count), replace=False)
    real_pickups = rng.choice(choosen_pickups, size=instance_size, replace=True)
    return real_pickups

# Generate random weights for each pickup-delivery request
def generate_weights(rng, matrix_size, instance_size):
    WEIGHT_MAX = 150
    return np.round(WEIGHT_MAX * rng.random(size=instance_size))

# Generate a time window map for a list of unique locations
def generate_time_window_map(rng, unique_locations):
    possible_starts = list(range(0, HORIZON, TIME_STEP))
    tw_map = {}

    for loc in unique_locations:
        while True:
            start = rng.choice(possible_starts)
            duration = rng.choice(DURATIONS)
            end = start + duration
            if end <= HORIZON:
                tw_map[loc] = (start, end)
                break

    return tw_map

# Ensure that for each pickup-delivery pair, the pickup time window starts before the delivery
def ensure_pickup_before_delivery(rng, pickup_locs, delivery_locs):
    pickup_tw_map = {}
    delivery_tw_map = {}
    
    for i, (pickup_loc, delivery_loc) in enumerate(zip(pickup_locs, delivery_locs)):
        # If this pickup location doesn't have a time window, generate
        if pickup_loc not in pickup_tw_map:
            possible_starts = list(range(0, HORIZON, TIME_STEP))
            while True:
                pickup_start = rng.choice(possible_starts)
                pickup_duration = rng.choice(DURATIONS)
                pickup_end = pickup_start + pickup_duration
                if pickup_end <= HORIZON:
                    pickup_tw_map[pickup_loc] = (pickup_start, pickup_end)
                    break
        
        pickup_start_for_this_pair = pickup_tw_map[pickup_loc][0]
        
        # If this delivery location doesn't have a time window, generate one that starts after pickup
        if delivery_loc not in delivery_tw_map:
            min_delivery_start = pickup_start_for_this_pair
            valid_starts = list(range(min_delivery_start, HORIZON, TIME_STEP))
            
            if not valid_starts:
                # If no valid time window, use pickup start as minimum
                valid_starts = [pickup_start_for_this_pair]
            
            while True:
                delivery_start = rng.choice(valid_starts)
                delivery_duration = rng.choice(DURATIONS)
                delivery_end = delivery_start + delivery_duration
                if delivery_end <= HORIZON:
                    delivery_tw_map[delivery_loc] = (delivery_start, delivery_end)
                    break
                # If a valid window cannot be created, reduce the duration
                if delivery_duration > DURATIONS[0]:
                    delivery_duration = DURATIONS[0]
                    delivery_end = delivery_start + delivery_duration
                    if delivery_end <= HORIZON:
                        delivery_tw_map[delivery_loc] = (delivery_start, delivery_end)
                        break
        else:
            # If delivery_loc already has a window, check that it meets the constraint for this pair
            existing_delivery_start = delivery_tw_map[delivery_loc][0]
            if existing_delivery_start < pickup_start_for_this_pair:
                # Conflict! Recreate a delivery time window that respects the constraint
                min_delivery_start = pickup_start_for_this_pair
                valid_starts = list(range(min_delivery_start, HORIZON, TIME_STEP))
                
                if not valid_starts:
                    valid_starts = [pickup_start_for_this_pair]
                
                while True:
                    delivery_start = rng.choice(valid_starts)
                    delivery_duration = rng.choice(DURATIONS)
                    delivery_end = delivery_start + delivery_duration
                    if delivery_end <= HORIZON:
                        delivery_tw_map[delivery_loc] = (delivery_start, delivery_end)
                        break
                    if delivery_duration > DURATIONS[0]:
                        delivery_duration = DURATIONS[0]
                        delivery_end = delivery_start + delivery_duration
                        if delivery_end <= HORIZON:
                            delivery_tw_map[delivery_loc] = (delivery_start, delivery_end)
                            break
    
    return pickup_tw_map, delivery_tw_map


def generate_instance(matrix_json_path, output_path, seed, matrix_size, instance_size):

    rng = np.random.default_rng(seed)
    full_matrix = load_matrix(matrix_json_path)

    pickups = generate_pickups(rng, matrix_size, instance_size)
    deliveries = generate_delivery(rng, matrix_size, instance_size)
    weights = generate_weights(rng, matrix_size, instance_size)

    pickup_tw_map, delivery_tw_map = ensure_pickup_before_delivery(rng, pickups, deliveries)

    os.makedirs(output_path, exist_ok=True)

    filename_txt = f"{output_path}/pdptw_instance_size_{instance_size*2}_seed_{seed}.txt"
    filename_json = f"{output_path}/pdptw_instance_size_{instance_size*2}_seed_{seed}.json"

    depot_id = 0
    
    # depot_geographic_position = rng.choice(range(matrix_size))
    while True:
        depot_geographic_position = rng.choice(range(matrix_size))
        if depot_geographic_position not in pickups and depot_geographic_position not in deliveries:
            break

    depot_coords = get_coordinates_from_matrix(full_matrix, depot_geographic_position)

    depot_etw = 0
    depot_ltw = HORIZON

    # Mapping : noeud_pdptw_id -> geographic position
    pdptw_node_to_geographic_position = {0: depot_geographic_position}

    for req_id, (p, d) in enumerate(zip(pickups, deliveries), start=1):
        pickup_node_id = req_id * 2 - 1
        delivery_node_id = req_id * 2
        
        pdptw_node_to_geographic_position[pickup_node_id] = p
        pdptw_node_to_geographic_position[delivery_node_id] = d

    # Generating the .txt
    with open(filename_txt, 'w') as f:
        f.write(f"{depot_id} 0 0 0 {depot_etw} {depot_ltw} 0 0 0\n")
        for req_id, (p, d, w) in enumerate(zip(pickups, deliveries, weights), start=1):

            p_tw = pickup_tw_map[p]
            d_tw = delivery_tw_map[d]
            
            assert p_tw[0] <= d_tw[0], f"Constraint violated: pickup {p} start={p_tw[0]} > delivery {d} start={d_tw[0]}"
            
            pickup_node_id = req_id * 2 - 1
            delivery_node_id = req_id * 2

            p_dem = int(w)
            d_dem = -int(w)

            f.write(f"{pickup_node_id} {p} {p} {p_dem} {p_tw[0]} {p_tw[1]} {SERVICE_DURATION} 0 {delivery_node_id}\n")
            f.write(f"{delivery_node_id} {d} {d} {d_dem} {d_tw[0]} {d_tw[1]} {SERVICE_DURATION} {pickup_node_id} 0\n")

    # Generating the .json
    json_result = {
        "size": instance_size * 2 + 1,
        "capacity": VEHICLE_CAPACITY,
        "depot": {
            "id": int(depot_id),
            "longitude": float(depot_coords[0]),
            "latitude": float(depot_coords[1]),
            "demand": 0,
            "timeWindow": [int(depot_etw), int(depot_ltw)],
            "serviceDuration": 0,
            "pairedLocation": 0,
            "locType": "DEPOT"
        },
        "locations": [],
        "matrix": []
    }

    for req_id, (p, d, w) in enumerate(zip(pickups, deliveries, weights), start=1):
        p_tw = pickup_tw_map[p]
        d_tw = delivery_tw_map[d]
        pickup_node_id = req_id * 2 - 1
        delivery_node_id = req_id * 2
        p_dem = int(w)
        d_dem = -int(w)

        # extract real coordinates from matrix json
        coord_pickup = get_coordinates_from_matrix(full_matrix, p)
        coord_delivery = get_coordinates_from_matrix(full_matrix, d)

        json_result["locations"].append({
            "id": int(pickup_node_id),
            "longitude": float(coord_pickup[0]),
            "latitude": float(coord_pickup[1]),
            "demand": int(p_dem),
            "timeWindow": [int(p_tw[0]), int(p_tw[1])],
            "serviceDuration": int(SERVICE_DURATION),
            "pairedLocation": int(delivery_node_id),
            "locType": "PICKUP"
        })

        json_result["locations"].append({
            "id": int(delivery_node_id),
            "longitude": float(coord_delivery[0]),
            "latitude": float(coord_delivery[1]),
            "demand": int(d_dem),
            "timeWindow": [int(d_tw[0]), int(d_tw[1])],
            "serviceDuration": int(SERVICE_DURATION),
            "pairedLocation": int(pickup_node_id),
            "locType": "DELIVERY"
        })

    # Create the PDPTW matrix
    pdptw_matrix = create_pdptw_matrix(full_matrix, pdptw_node_to_geographic_position)
    json_result["matrix"] = pdptw_matrix

    with open(filename_json, 'w') as fjson:
        json.dump(json_result, fjson, indent=2)

    print(f"Generated files:: {filename_txt} et {filename_json}")
    print(f"PDPTW matrix size: {len(pdptw_matrix)}x{len(pdptw_matrix[0])}")
    print(f"Mapping PDPTW nodes to geographic positions: {pdptw_node_to_geographic_position}")


def create_pdptw_matrix(full_matrix, node_to_position_map):

    # Get all PDPTW nodes in order
    pdptw_nodes = sorted(node_to_position_map.keys())
    matrix_size = len(pdptw_nodes)
    
    pdptw_matrix = []
    
    for origin_node in pdptw_nodes:
        row = []
        origin_position = node_to_position_map[origin_node]
        
        for dest_node in pdptw_nodes:
            dest_position = node_to_position_map[dest_node]
            
            # Extract the entry from the full matrix
            if (origin_position < len(full_matrix) and 
                dest_position < len(full_matrix[origin_position])):
                entry = full_matrix[origin_position][dest_position]
                
                if isinstance(entry, dict):
                    # Copy the entry and update the IDs to match the PDPTW nodes
                    new_entry = entry.copy()
                    new_entry['origin'] = origin_node
                    new_entry['destination'] = dest_node
                    row.append(new_entry)
                else:
                    row.append(None)
            else:
                row.append(None)
        
        pdptw_matrix.append(row)
    
    return pdptw_matrix


def get_coordinates_from_matrix(matrix, location_id):
    try:
        entry = matrix[location_id][location_id]  # diagonal: origin == destination
        return entry["c_origin"][:2]  # longitude, latitude
    except (IndexError, KeyError, TypeError):
        return [0.0, 0.0]


def load_matrix(json_file):
    with open(json_file, 'r') as f:
        matrix = json.load(f)
    return matrix


def generate_all_instances():
    matrix_size = 1500
    instance_number = 1
    instance_size = 10
    matrix_json_path = "elevation_matrix/elevation_matrix.json"
    for instance_size in [10, 25, 50, 100]: #nb of request !
        for instance_number in range(10):
            generate_instance(matrix_json_path, "instances_pdp_elevation", instance_number, matrix_size, instance_size)
