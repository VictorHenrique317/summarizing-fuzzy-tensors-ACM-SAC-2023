function [rowSetsPruned, colSetsPruned] = allCombinations(A, rowSets, colSets)
display('Consider every combination of rows and columns.');
% For each row set find the best column set.
blockIdx = 1;
for i = 1 : numel(rowSets)
    i
    for j = 1 : numel(colSets)
        rowSetsPruned{blockIdx} = rowSets{i};
        colSetsPruned{blockIdx} = colSets{j};       
        blockIdx = blockIdx + 1
    end
end

end