# ==========================================================
# Script :      txt_to_json.jl
# Description:  Read the PDPTW instance (.txt) LL01 of Li and Lim(2003) and SB19 of Sartori and Buriol (2020)
#               and convert them into uniform JSON (.json) file
#               /!\ Each directory needs to contain only one type of instance (LL01 or SB19) and no other files
#               
# ==========================================================

# txt_to_json.jl 
# Data_txt/ 
# | n100/  
# | | bar-n100-1.txt
# | | ...
# | pdp_100/ 
# | | lc101.txt
# | | ...
# Data_json/


using DelimitedFiles
using JSON
using OrderedCollections
using Glob

struct Location
    id::Int
    longitude::Float64
    latitude::Float64
    demand::Int
    timeWindow::Vector{Int64}
    serviceDuration::Int
    pairedLocation::Int
    locType::String
end

# read LL01 instance and return nb_vehicles, capacity, locations
function read_pdptw_LL01_instance(filename)
    open(filename, "r") do file
        lines = readlines(file) 
        first_line = split(strip(lines[1]))

        if length(first_line) < 3
            error("Error: line 1")
        end

        capacity = parse(Int, first_line[2])

        locations = Location[]

        for i in 2:length(lines)
            row = split(strip(lines[i]))
            if length(row) < 9
                error("Error: line $(i)")
            end

            timeWindow = [parse(Int, row[5]), parse(Int, row[6])]

            # depot
            if  (parse(Int, row[8]) == 0) && (parse(Int, row[9]) == 0)
                locType = "DEPOT"
                pairedLocation = 0
            elseif (parse(Int, row[8]) == 0)
                locType = "PICKUP"
                pairedLocation = parse(Int, row[9])
            elseif (parse(Int, row[9]) == 0)
                locType = "DELIVERY"
                pairedLocation = parse(Int, row[8])
            end


            push!(locations, Location(parse(Int, row[1]), 
                                    parse(Float64, row[2]), 
                                    parse(Float64, row[3]),
                                    parse(Int, row[4]), 
                                    timeWindow, 
                                    parse(Int, row[7]),
                                    pairedLocation, 
                                    locType))
        end

        return capacity, locations
    end
end

# read SB19 instance and return nb_vehicles, capacity, locations, distance_matrix
function read_pdptw_SB19_instance(filename)
    open(filename, "r") do file
        lines = readlines(file)

        size = parse(Int, split(strip(lines[5]))[2])
        capacity = parse(Int, split(strip(lines[10]))[2])

        locations = Location[]

        for i in 12:size+11
            row = split(strip(lines[i]))
            if length(row) < 9
                error("Error: line $(i)")
            end

            timeWindow = [parse(Int, row[5]), parse(Int, row[6])]

            # depot
            if  (parse(Int, row[8]) == 0) && (parse(Int, row[9]) == 0)
                locType = "DEPOT"
                pairedLocation = 0
            elseif (parse(Int, row[8]) == 0)
                locType = "PICKUP"
                pairedLocation = parse(Int, row[9])
            elseif (parse(Int, row[9]) == 0)
                locType = "DELIVERY"
                pairedLocation = parse(Int, row[8])
            end

            push!(locations, Location(parse(Int, row[1]), 
                                    parse(Float64, row[2]), 
                                    parse(Float64, row[3]),
                                    parse(Int, row[4]), 
                                    timeWindow,
                                    parse(Int, row[7]), 
                                    pairedLocation,
                                    locType))
        end
        
        # travel time taken from a map by Sartori and Buriol
        distance_matrix = Array{Float64}(undef, size, size)
        
        # read the distance matrix (better way to do it but that is working...)
        cpt = 1
        for i in size+13:2*size+12
            row = split(strip(lines[i]))
            if length(row) < size
                error("Error: line $(i)")
            end
            for j in eachindex(row)
                distance_matrix[cpt, j] = parse(Float64, row[j])
            end
            cpt = cpt+1
        end

        return size, capacity, locations, distance_matrix
    end
end

# given an output directory, a filename and the PDPTW data, create a json file
function save_to_json(output_dir, filename, size, capacity, depot, locations, distance_matrix)

    # correction to store the matrix row by row and not column by column in the json file
    matrix_row = collect(eachrow(distance_matrix))

    data = OrderedDict(
        "size" => size,
        "capacity" => capacity,
        "depot" =>
            OrderedDict(
                "id" => depot.id,
                "longitude" => depot.longitude,
                "latitude" => depot.latitude,
                "demand" => depot.demand,
                "timeWindow" => depot.timeWindow,
                "serviceDuration" => depot.serviceDuration,
                "pairedLocation" => depot.pairedLocation,
                "locType" => depot.locType
            )
        ,
        "locations" => [
            OrderedDict(
                "id" => r.id,
                "longitude" => r.longitude,
                "latitude" => r.latitude,
                "demand" => r.demand,
                "timeWindow" => r.timeWindow,
                "service_duration" => r.serviceDuration,
                "pairedLocation" => r.pairedLocation,
                "locType" => r.locType
            ) for r in locations
        ],
        "distance_matrix" => matrix_row
    )


    output_file = joinpath(output_dir, replace(basename(filename), ".txt" => ".json"))

    open(output_file, "w") do file
        write(file, JSON.json(data, 4))
    end
end


function euclidean_distance(x1, y1, x2, y2)
    return round(sqrt((x2 - x1)^2 + (y2 - y1)^2), digits=2)
end


# Distance matrix
function compute_distance_matrix(locations)
    n = length(locations)
    distance_matrix = Array{Float64}(undef, n, n)

    for i in 1:n
        for j in 1:n
            distance_matrix[i, j] = euclidean_distance(
                locations[i].longitude, locations[i].latitude,
                locations[j].longitude, locations[j].latitude
            )
        end
    end

    return distance_matrix
end

# convert all the files of a given type of PDPTW instances in input_dir and store them in output_dir
function process_all_files(input_dir, output_dir, type)

    if !isdir(output_dir)
        mkdir(output_dir)
    end

    txt_files = glob("*.txt", input_dir)

    for file in txt_files
        println("Reading $(file)...")

        try
            if type == "LL01"
                capacity, locations = read_pdptw_LL01_instance(file)
                size = length(locations)
                distance_matrix = compute_distance_matrix(locations)  # simple euclidian distance
                depot = popfirst!(locations)
                save_to_json(output_dir, file, size, capacity, depot, locations, distance_matrix)
            end
            if type == "SB19"
                size, capacity, locations, distance_matrix = read_pdptw_SB19_instance(file)
                depot = popfirst!(locations)
                save_to_json(output_dir, file, size, capacity, depot, locations, distance_matrix)
            end
            
        catch e
            println("Error $(file) : ", e)
        end
    end

    println("Success $(input_dir).")
end

function find_all_directory(parent_dir)
    return [folder for folder in glob("*/", parent_dir) if isdir(folder)]
end


########################################################
########################################################

# txt_to_json.jl 
# Data_txt/ 
# | n100/  
# | | bar-n100-1.txt
# | | ...
# | pdp_100/ 
# | | lc101.txt
# | | ...
# ...
# Data_json/

data_directory = "Data_txt"

all_directory = find_all_directory(data_directory)


for dir in all_directory
    println("--- \n", dir)
    if startswith(dir, "Data_txt/pdp_")
        input_directory = dir
        output_directory = replace(dir, "txt" => "json")
        process_all_files(input_directory, output_directory, "LL01")    
    elseif startswith(dir, "Data_txt/n")
        input_directory = dir
        output_directory = replace(dir, "txt" => "json")
        process_all_files(input_directory, output_directory, "SB19")
    else
        println("File not compatible")
    end
end
