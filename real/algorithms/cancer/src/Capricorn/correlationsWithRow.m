function Corrs_ = correlationsWithRow(A, row, bucketSizeThresh, delta)
rowRejectionThreshold = 0.5;
idx = isnan(A);
A(idx) = 0;

[n, m] = size(A);
Corrs_ = getCorrs(A, row, bucketSizeThresh, delta);


%% cluster rows based on dot product
sm = sum(Corrs_, 2);
[~, idx] = sort(sm, 'descend');
secondHeaviestRow = idx(2);
h = findRowSets(A, bucketSizeThresh, delta, row, secondHeaviestRow);
if numel(h) == 0
    Corrs_ = zeros(size(A));
else
    ids = h{1};
    Corrs_(row, :) = 0;
    Corrs_(row, ids) = 1;
    
    dots = nan(n, 1);
    for i = 1 : n
        dots(i) = dot(Corrs_(i,:), Corrs_(row,:)) / (nnz(Corrs_(i,:))+1);
    end
    sorted = sort(dots, 'descend');
    t = sorted(2);
    firstBlockIdx = [];
    for  i = 1 : n
        if dots(i) > t - rowRejectionThreshold
            firstBlockIdx = [firstBlockIdx; i];
        end
    end
    
    cancelRows = setdiff(1:n, firstBlockIdx);
    Corrs_(cancelRows, :) = 0;
end

end


function Corrs_ = getCorrs(A, row, bucketSizeThresh, delta)
[n, ~] = size(A);
Corrs_ = zeros(size(A));
for i = 1 : n
    newColSets = findRowSets(A, bucketSizeThresh, delta, row, i);
    for j = 1 : numel(newColSets)
        Corrs_(i, newColSets{j}) = 1;
%         imagesc(Corrs_)
    end
end
end