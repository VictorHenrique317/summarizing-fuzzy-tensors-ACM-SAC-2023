function [bIdx, cIdx] = getPattern(Corrs)
if nnz(Corrs) == 0
    bIdx = [];
    cIdx = [];
else
    kappa = 0.1;
    sm = sum(Corrs, 2);
    [~, idxC] = max(sm);
    sm = sum(Corrs, 1);
    [~, idxB] = max(sm);
    bIdx = find(Corrs(:, idxB));
    cIdx = find(Corrs(idxC, :));
    % Remove rows and columns that are underfilled with ones.
    [n, m] = size(Corrs);
    Pattern = zeros(n, m);
    Pattern(bIdx, cIdx) = 1;
    
    toRemoveB = [];
    for i = 1 : n
        saturation = nnz(Corrs(i, :)) / nnz(Pattern(i, :));
        if saturation < kappa
            toRemoveB = [toRemoveB, i];
        end
    end
    bIdx = setdiff(bIdx, toRemoveB);
    
    toRemoveC = [];
    for j = 1 :m
        saturation = nnz(Corrs(:, j)) / nnz(Pattern(:, j));
        if saturation < kappa
            toRemoveC = [toRemoveC, j];
        end
    end
    cIdx = setdiff(cIdx, toRemoveC);
end

end

