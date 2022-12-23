% matlab -nodisplay -r 'cd("."); cancer(8,"../../iterations/1/tensors/plain/matrix9.txt","../../iterations/1","co1-u0.6");exit' | tail -n +11
function [] = cancer(rank, mat_path, current_iteration_folder, current_experiment)
addpath('../../algorithms/cancer/src/Product')
addpath('../../algorithms/cancer/src/Metrics')
addpath('../../algorithms/cancer/src/Cancer')

matrix = load(mat_path).matrix;
% matrix = str2num(matrix);

tic
[B, C] = Cancer(matrix, rank);
elapsed_time = toc;

temp_folder = current_iteration_folder + "/output/" + current_experiment + "/experiments/temp";
mkdir(temp_folder);

pattern_nb = 0;
% for i = 1 : size(matrix, 1)
for i = 1 : rank
    B_ = B(:,i);
    C_ = C(i,:);
    pattern = MaxProduct(B_, C_);
 
    if sum(abs(pattern(:))) > 0 
        pattern_nb = pattern_nb + 1;
        pattern_file_path = temp_folder + "/pattern_" + i + ".txt";
        file_id = fopen(pattern_file_path, "w");
        pattern = mat2str(pattern);
        fprintf(file_id, pattern);
    end
end

log_path = current_iteration_folder + "/output/" + current_experiment + "/logs/cancer.log";
file_id = fopen(log_path, "w");
fprintf(file_id, "filler:0\n");
fprintf(file_id, "filler:0\n");
fprintf(file_id, "filler:0\n");

pattern_nb = "Nb of patterns: " + pattern_nb + "\n";
fprintf(file_id, pattern_nb);

elapsed_time = "Run time: " + elapsed_time + '\n';
fprintf(file_id, elapsed_time);
end
