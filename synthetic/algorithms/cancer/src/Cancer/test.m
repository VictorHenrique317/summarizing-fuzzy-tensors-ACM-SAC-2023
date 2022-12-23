
% A = [0 0 0 0; 0 0 0 0; 0 0 1 0]
if sum(abs(A(:))) > 0
    disp("not null")
end

path = "../../iterations/1/tensors/plain/matrix9.txt";
%filetext = fileread(path);
%filetest = str2num(filetext)

addpath('../Product')
addpath('../Capricorn')
addpath('../Metrics')

matrix1 =  [0.9 0.8 0.1 0.2; 0.7 .6 0.1 0.3; 0.2 0.1 0.8 0.9; 0.3 0.2 0.8 0.7];
matrix2 =  [1 1 0 0; 1 1 0 0; 0 0 1 1; 0 0 1 1];
matrix3 = [1 1 1 1 1 1 1 1 1; 1 1 1 1 1 1 0 0 0; 1 1 1 1 1 1 0 0 0; 1 1 1 1 1 0 0 0 0; 1 1 1 1 1 0 0 0 0];
matrix4 = [0.88 0.97 0.78 0.68 0.84 0.98 0.67 0.86 0.92; 0.81 0.76 0.82 0.83 0.97 0.76 0.26 0.12 0.47; 0.89 0.72 0.67 0.79 0.91 0.87 0.42 0.21 0.11; 0.85 0.72 0.85 0.96 0.75 0.15 0.28 0.14 0.16; 0.89 0.84 0.86 0.96 0.89 0.35 0.26 0.19 0.25];
matrix5 = [2 3 4 1 6 2 7 1 6; 4 2 6 4 3 1 0 0 0; 2 4 6 4 6 8 0 0 0; 4 6 7 1 1 0 0 0 0; 2 4 3 6 2 0 0 0 0];
matrix6 = [1 2 0; 2 4 1; 0 4 2];
matrix7 = randi([0, 5], [10,10]);
matrix7 = abs(sin(matrix7));
matrix8 = [1 1 1 1 0 0 0 0 0 0 0; 1 1 1 1 0 0 0 0 0 0 0; 1 1 1 1 0 0 0 0 0 0 0; 0 0 0 0 0 0 0 0 0 0 0; 0 0 0 0 0 0 0 0 0 0 0;
    0 0 0 0 0 0 0 1 1 1 1; 0 0 0 0 0 0 0 1 1 1 1; 0 0 0 0 0 0 0 1 1 1 1; 0 0 0 0 0 0 0 1 1 1 1; 0 0 0 0 0 0 0 1 1 1 1];
matrix9 = [1 2 4 3 0.5 0.3 0.2 0.1 0.3 0.1 0.4; 8 2 1 4 0.2 0.1 0.1 0.3 0.4 0.2 0.1; 5 2 4 3 0.1 0.2 0.3 0.1 0 0 0; 0 0 0 0 0 0 0 0 0 0 0; 0 0 0 0 0 0 0 0 0 0 0;
    0 0 0 0 0 0 0 3 2 5 4; 0 0 0 0 0 0 0 2 4 3 5; 0 0 0 0 0 0 0 5 2 3 4; 0 0 0 0 0 0 0 2 4 5 3; 0 0 0 0 0 0 0 5 4 4 3];
matrix9 = abs(sin(matrix9));
matrix10 = [1 1 0 0; 1 1 0 0; 0 0 1 1; 0 0 1 1];
matrix11 = [1 0; 0 1];
rank=8;
[B, C] = Cancer(matrix8, rank, 'NCYCLES', 40);

%[B, C] = Cancer(matrix6, rank, 'NCYCLES', 100);
matrix8
B
C

% 10r x 11c
disp("disping outers");
% indices start at 1
for i = 1 : 2
    B_ = B(:,i);
    C_ = C(i,:);
    outer = MaxProduct(B_, C_)
end


%%
addpath('../Product')

rank = 2;

[B, C] = Cancer([1 0; 0 1], 2, 'NCYCLES', 40);
B
C

%matrix9_str = mat2str(matrix9)
%matrix9_conv = str2num(matrix9_str)
