using CSV, DataFrames, StatsBase, Random, Distributions

input = "/home/a24jacqb/Documents/Code/pdptw-main/julia/LCN_01.csv"

df = CSV.read(input, DataFrame)

# keep only some columns
keep = ["# order", "orderRevenue", "type", "address.latlng", "afterDay", "afterTime", "beforeDay", "beforeTime", "finishedAtDay", "finishedAtTime", "comments"]

select!(df, keep)

# retrieve some usefull informations in "comments"
function extract_weight(text)
    text = string(text)
    if ismissing(text)
        return ""
    end
    m = match(r"\d+\.\d+\s*kg", text)
    m = (m !== nothing) ? m.match : ""
    return (m == "0.00 kg") ? "" : m 
end

df.weight = map(extract_weight, df.comments)
df.weight = convert(Vector{Union{Missing, String}}, df.weight)
df.weight = replace!(df.weight, "" => missing)

df.weight = map(x -> ismissing(x) ? missing : replace(x, r"\s*kg" => ""), df.weight)
df.weight = map(x -> ismissing(x) ? missing : parse(Float64, x), df.weight)

select!(df, Not("comments")) # delete "comments"

function count_is_missing(column)
    return count(x -> ismissing(x), column)
end


##################### modify / generate data

# remove doublon
df = unique(df)

# remove missing # order
dropmissing!(df, ["# order"])

# fill missing data (weight & revenue) with a normal distribution

function fill_missing(value, type)
    if ismissing(value) && type == "PICKUP"
        new_value = round(rand(Normal(mu, sigma)), digits=2)
        new_value = max(new_value, 1.0)
        new_value = min(new_value, maxi)
        return new_value
    else
        return value
    end
end


maxi = maximum(skipmissing(df.weight))
mu = mean(skipmissing(df.weight))
sigma = std(skipmissing(df.weight))

df.weight = map(fill_missing, df.weight, df.type)


for row in eachrow(df)
    if row.type == "DROPOFF"
        row.weight = - df.weight[(df."# order" .== row."# order") .& (df.type .== "PICKUP")][1]
    end
end

println(first(df, 15))

#CSV.write("clean_LCN_01.csv", df)