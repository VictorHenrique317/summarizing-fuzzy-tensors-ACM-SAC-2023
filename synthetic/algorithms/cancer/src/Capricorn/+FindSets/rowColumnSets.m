function [rowSets, colSets] = rowColumnSets(A, bucketSizeThresh, delta, nSeedRows)
% From row correlations we recover column sets and vice versa.
% Hence rows and column have to be inversed.
display('find row and column sets');
colSets = {};
rowSets = {};
[n, m] = size(A);
for i = 1 : nSeedRows
    fprintf('processing seed number  %d\n', i);
    [row1, row2] = selectRows(n);
    %debug
    Corrs_ = correlationsWithRow(A, row1);
    %debug
    newColSets = findRowSets(A, bucketSizeThresh, delta, row1, row2);
    colSets = [colSets, newColSets];
    [col1, col2] = selectRows(m);
    NewRowSets = findRowSets(A', bucketSizeThresh, delta, col1, col2);
    rowSets = [rowSets, NewRowSets];
end
rowSets = Prune.removeDuplicates(rowSets);
colSets = Prune.removeDuplicates(colSets);

end


function [row1, row2] = selectRows(n)
row1 = randi(n);
row2 = randi(n);
while row1 == row2
    row2 = randi(n);
end
end