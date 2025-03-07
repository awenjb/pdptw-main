import json
import datetime
import enum

import folium
import numpy as np
import pandas as pd
import osmnx as ox
import networkx as nx

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


