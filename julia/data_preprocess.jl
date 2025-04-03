using CSV
using DataFrames


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

select!(df, Not("comments")) # delete "comments"

println(first(df, 15))


function count_is_missing(column)
    return count(x -> ismissing(x), column)
end

# remove doublon

# remove empty order

# fill missing data (weight & revenue) with a normal distribution

CSV.write("clean_LCN_01.csv", df)