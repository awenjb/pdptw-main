import json
import datetime
import enum

import folium
import numpy as np
import pandas as pd
import osmnx as ox
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors

def read_best_known(file):
    try:
        with open(file, 'r') as file:
            df = pd.read_csv(file, delim_whitespace=True, names=["Instance", "Vehicles", "Distance", "Reference", "Date"], skiprows=1)
            return df
    except FileNotFoundError:
        print(f"Error : The file {file} does not exist.")
    except Exception as e:
        print(f"Error : {e}")


def read_json(file):
    try:
        with open(file, 'r') as file:
            data = json.load(file)
            return data
    except FileNotFoundError:
        print(f"Error : The file {file} does not exist.")
    except json.JSONDecodeError:
        print(f"Error : The file {file} is not a valid JSON file.")
    except Exception as e:
        print(f"Error : {e}")


G_drive = ox.graph_from_point((47.213811, -1.553168), dist=10000,
                              network_type='bike')

class PDPTWSolution:
    def __init__(self, filename: str):
        with open(filename, mode="r", encoding="utf-8") as f:
            self._json_file = json.load(f)
        self._parse_json()

    def _parse_json(self):
        if self._json_file is None:
            raise "Trying to parse json, but was not loaded"
        self._routes: list[dict] = self._json_file["routes"]

    def get_routes(self):
        return self._routes
    
    def get_json(self):
        return self._json_file

class PDPTWCompleteSolution:
    def __init__(self, filename: str):
        with open(filename, mode="r", encoding="utf-8") as f:
            self._json_file = json.load(f)
        self._parse_json()

    def _parse_json(self):
        if self._json_file is None:
            raise "Trying to parse json, but was not loaded"
        self._routes: list[dict] = self._json_file["routes"]
        self._vehicles = len(self._routes)
        self._cost = self._json_file["cost"]
        self._instance = self._json_file["instanceName"]
        self._iteration = self._json_file["iteration"]
        self._time = self._json_file["time"]
        self._date = self._json_file["date"]
        self._reference = self._json_file["reference"]

    def get_routes(self):
        return self._routes
    
    def get_vehicles(self):
        return self._vehicles

    def get_cost(self):
        return self._cost
    
    def get_instance(self):
        return self._instance
    
    def get_iteration(self):
        return self._iteration

    def get_time(self):
        return self._time
    
    def get_date(self):
        return self._date

    def get_reference(self):
        return self._reference
    
    def get_json(self):
        return self._json_file
    

class PDPTWData:
    def __init__(self, filename: str):
        with open(filename, mode="r", encoding="utf-8") as f:
            self._json_file = json.load(f)
        self._parse_json()

    def _parse_json(self):
        if self._json_file is None:
            raise "Trying to parse json, but was not loaded"
        self._locations: list[dict] = self._json_file["locations"]
        self._depot: list[dict] = self._json_file["depot"]
        self._capacity = self._json_file["capacity"]

    def get_depot(self):
        return self._depot
    
    def get_locations(self):
        return self._locations

    def get_capacity(self):
        return self._capacity

    def get_json(self):
        return self._json_file


def get_route_as_dataframe(data: PDPTWData, solution: PDPTWSolution, route_index:int) -> pd.DataFrame:
    route_df=pd.DataFrame()
    route_ids = solution.get_routes()[route_index]["locationIDs"]
    for id in route_ids:
        line = {"route": route_index, 
                "location": id,
                "type": data.get_locations()[id - 1]["locType"],
                "latitude": data.get_locations()[id - 1]["latitude"],
                "longitude": data.get_locations()[id - 1]["longitude"]}
        route_df = pd.concat([route_df, pd.DataFrame.from_dict([line])], ignore_index=True)
    return route_df

def display_points_of_interest(map: folium.Map, data: PDPTWData):
    folium.Marker(location=[data.get_depot()["latitude"], data.get_depot()["longitude"]],
                    icon=folium.Icon(color="red", popup="DEPOT")).add_to(map)
    for location in data.get_locations():
        if location["locType"] == "PICKUP":
            color = "darkblue"
            text = "pickup"
        elif location["locType"] == "DELIVERY":
            color = "lightblue"
            text = "delivery"
        else:
            continue
        folium.Marker(location=[location["latitude"], location["longitude"]],
                      icon=folium.Icon(color=color, popup=text)).add_to(map)
        
def display_route(map: folium.Map, data: PDPTWData, solution: PDPTWSolution, route_index: int, color: str="red"):
    df = get_route_as_dataframe(data, solution, route_index)
    # add depot at the begining and the end
    types = ["DEPOT"] + list(df["type"]) + ["DEPOT"]
    depot_coord = (data.get_depot()["latitude"], data.get_depot()["longitude"])
    locations = [depot_coord] + list(zip(df["latitude"], df["longitude"])) + [depot_coord]
    real_path_locations = []
    network_locations = ox.nearest_nodes(G_drive,
                                         [data.get_depot()["longitude"]] + list(df["longitude"]) + [data.get_depot()["longitude"]],
                                         [data.get_depot()["latitude"]] + list(df["latitude"]) + [data.get_depot()["latitude"]])

    for i in range(1, len(locations)):
        edge = [network_locations[i - 1], network_locations[i]]
        real_nodes = ox.shortest_path(G_drive, edge[0], edge[1], weight="length")
        if real_nodes is not None:
            long_lat_edges = []
            for node in real_nodes:
                g_drive_node = G_drive.nodes[node]
                if "x" in g_drive_node and "y" in g_drive_node:
                    long_lat_edges.append((g_drive_node["y"], g_drive_node["x"]))
            real_path_locations.extend(long_lat_edges)
        else:
            real_path_locations.append(locations[i])

    folium.PolyLine(real_path_locations, color=color, weight=5, opacity=0.5, smooth_factor=0).add_to(map)
    for index, row in df.iterrows():
        if row["type"] == "REQUEST":
            text = f'{index} - {row["latitude"], row["longitude"]} : times ({row["arrival"]},{row["service"]},{row["departure"]}), time window [{row["Hmin"]},{row["Hmax"]}]'
            folium.Circle(location=(row["latitude"], row["longitude"]), fill_color="orange", radius=4,
                          tooltip=text).add_to(map)


def plot_number_columns(routes, data):

    sequences = []
    location_ids_list = [] 
    # recalculate the capacity with de route and data
    for route in routes :
        ids = route["locationIDs"]
        cumulated = []
        for id in ids :
            location = data.get_locations()[id - 1]
            demand = location["demand"]
            if not cumulated:
                cumulated.append(demand)
            else :
                cumulated.append( cumulated[-1] + demand)
        sequences.append(cumulated)
        location_ids_list.append(ids)

    num_sequences = len(sequences)
    fig, axes = plt.subplots(num_sequences, 1, figsize=(10, num_sequences * 4), sharex=False)
    
    #fig.suptitle("Capacity Constraint", fontsize=16, fontweight='bold')

    if num_sequences == 1:
        axes = [axes]
    
    for i, (ax, seq, loc_ids) in enumerate(zip(axes, sequences, location_ids_list)):

        max_value = data.get_capacity()
        
        #normalized_seq = [value / max_value * 100 for value in seq] # %
        x_positions = np.arange(len(seq))
        
        norm = mcolors.Normalize(vmin=min(seq), vmax=max_value)
        colors = [cm.get_cmap('YlOrRd')(norm(value)) for value in seq]
        
        ax.bar(x_positions, seq, color=colors, edgecolor='black', width=1.0) 
        ax.set_ylabel('Used capacity leaving the location')
        ax.set_title(f'Route {i+1}')
        #ax.set_ylim(0, 110)
        ax.set_ylim(0, max_value + max_value * 0.1)
        ax.set_xticks(x_positions)
        ax.set_xticklabels(loc_ids, rotation=0)

        ax.set_xlabel('Location ID')

    plt.tight_layout()
    plt.show()