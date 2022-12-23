function [B, C] = simAnnealFactors(A, rows, cols)
blocks = chooseBlocks(A, rows, cols);
B = zeros(n, 0);
C = zeros(0, m);
idx = find(blocks);
for i = 1 : numel(idx)
    B = [B, rows{idx(i)}];
    C = [C; cols{idx(i)}];
end

end