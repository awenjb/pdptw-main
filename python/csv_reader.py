import json
import os
from typing import OrderedDict
import pandas as pd
import osmnx as ox
import networkx as nx
from datetime import datetime

class Location:
    def __init__(self, id, longitude, latitude, demand, time_window,
                 service_duration, paired_location, loc_type):
        self._id = id
        self._longitude = longitude
        self._latitude = latitude
        self._demand = demand
        self._time_window = time_window
        self._service_duration = service_duration
        self._paired_location = paired_location
        self._loc_type = loc_type

    def get_id(self):
        return self._id

    def get_longitude(self):
        return self._longitude
    
    def get_latitude(self):
        return self._latitude
    
    def get_demand(self):
        return self._demand

    def get_time_window(self):
        return self._time_window
    
    def get_service_duration(self):
        return self._service_duration

    def get_paired_location(self):
        return self._paired_location
    
    def get_loc_type(self):
        return self._loc_type


def compute_matrix(locations, avg_speed_kmh=15):
    coords = [(loc.get_latitude(), loc.get_longitude()) for loc in locations]

    center_lat = locations[0].get_latitude()
    center_lon = locations[0].get_longitude()

    G = ox.graph_from_point((center_lat, center_lon), dist=10000, network_type='bike')

    node_ids = ox.nearest_nodes(G, [lon for _, lon in coords], [lat for lat, _ in coords])

    n = len(node_ids)
    distance_matrix = [[0.0] * n for _ in range(n)]
    time_matrix = [[0.0] * n for _ in range(n)] 
    
    for i in range(n):
        for j in range(n):
            if i == j:
                distance_matrix[i][j] = 0.0
                time_matrix[i][j] = 0.0
                continue
            try:
                dist = nx.shortest_path_length(G, node_ids[i], node_ids[j], weight='length')
                distance_matrix[i][j] = (dist / 1000.0)
                time_matrix[i][j] = ((dist / 1000.0) / avg_speed_kmh) * 3600 # hours to seconds
            except nx.NetworkXNoPath:
                distance_matrix[i][j] = float("inf")
                time_matrix[i][j] = float("inf")

    return distance_matrix, time_matrix


def save_to_json(output_dir, filename, size, capacity, depot, locations, distance_matrix):
    data = OrderedDict({
        "size": size,
        "capacity": capacity,
        "depot": OrderedDict({
            "id": depot.get_id(),
            "longitude": depot.get_longitude(),
            "latitude": depot.get_latitude(),
            "demand": depot.get_demand(),
            "timeWindow": depot.get_time_window(),
            "serviceDuration": depot.get_service_duration(),
            "pairedLocation": depot.get_paired_location(),
            "locType": depot.get_loc_type()
        }),
        "locations": [OrderedDict({
            "id": loc.get_id(),
            "longitude": loc.get_longitude(),
            "latitude": loc.get_latitude(),
            "demand": loc.get_demand(),
            "timeWindow": loc.get_time_window(),
            "serviceDuration": loc.get_service_duration(),
            "pairedLocation": loc.get_paired_location(),
            "locType": loc.get_loc_type()
        }) for loc in locations],
        "distance_matrix": distance_matrix
    })

    output_path = os.path.join(output_dir, os.path.basename(filename) + ".json")
    with open(output_path, "w") as f:
        json.dump(data, f, indent=4)


def time_to_seconds(t: str) -> int:
    h, m, s = map(float, t.split(":"))
    return int(h * 3600 + m * 60 + s)


def read_day_requests(csv_file: str, target_day: str) -> list[Location]:
    
    df = pd.read_csv(csv_file, sep=',')
    df = df[df['afterDay'] == target_day]

    # Locations
    locations = []
    for _, row in df.iterrows():

        location_id = int(row['# order']) if not pd.isna(row['# order']) else None

        if (row['type']=="PICKUP"):
            loc_type = "PICKUP"
            paired_location = (len(locations)+1) + 1
        else :
            loc_type = "DELIVERY"
            paired_location = (len(locations)+1) - 1

        
        lat, lon = map(float, row['address.latlng'].split(','))
        demand = float(row['weight'])

        start_time = time_to_seconds(str(row['afterTime']))
        end_time = time_to_seconds(str(row['beforeTime']))

        loc = Location(id=(len(locations)+1), longitude=lon, latitude=lat, demand=demand,
                        time_window=[start_time, end_time], service_duration=300, 
                        paired_location=paired_location, loc_type=loc_type)

        locations.append(loc)


    # Depot
    start_day = time_to_seconds(str(min(df['afterTime']))) - 1200 
    end_day = time_to_seconds(str(max(df['beforeTime']))) + 1200
    depot = Location(id=0, longitude=-1.5461537253097524, latitude=47.20425475808151, demand=0,
                    time_window=[start_day, end_day], service_duration=0,
                    paired_location=0, loc_type="DEPOT")
    
    return locations, depot

locations, depot = read_day_requests("julia/clean_LCN_01.csv", "03/10/2023")

size = len(locations) + 1
capacity = 275

copyLocations = list(locations)
copyLocations.insert(0, depot)

distance_matrix, time_matrix = compute_matrix(copyLocations)

save_to_json("data_in/Nantes/", "Nantes_03_10_2023", size, capacity, depot, locations, time_matrix)
