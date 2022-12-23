args = commandArgs()
translated_tensor_path = args[6]
noise_endurance = as.double(args[7])
max_pattern_number = as.integer(args[8])
iteration = as.integer(args[9])
experiment_folder_name = args[10]
numpy_name = args[11]

source("../libs/GETF/GETF_CP.R")
library("reticulate")

np <- import("numpy")
tensor <- np$load(translated_tensor_path)

start_time <- Sys.time()
Factors<-GETF_CP(TENS = tensor, Thres = noise_endurance, B_num = max_pattern_number, COVER = 0.9, Exhausive = T)
end_time <- Sys.time()

time_spent <- difftime(end_time, start_time, units = "secs")[[1]]
Patterns <- Get_Patterns(tensor, Factors)
for(i in 1:length(Patterns)){
    if (length(Patterns) != 0){
        Pattern <- Patterns[[i]]
        pattern_file_path = paste("../experiment/iterations/", iteration, "/", experiment_folder_name, "/getf/p", i, "-",numpy_name)
        pattern_file_path = gsub(" ", "", pattern_file_path)
        np$save(pattern_file_path, Pattern)
    }
}

log_file_path = paste("../experiment/iterations/", iteration, "/", experiment_folder_name, "/getf/log.txt")
log_file_path = gsub(" ", "", log_file_path)

pattern_nb <- paste("Nb of selected patterns:", length(Patterns))
time_spent <- paste("Total time:", time_spent)

file.create(log_file_path)
log_file <- file(log_file_path)
writeLines(c(pattern_nb, time_spent), log_file)
close(log_file)





