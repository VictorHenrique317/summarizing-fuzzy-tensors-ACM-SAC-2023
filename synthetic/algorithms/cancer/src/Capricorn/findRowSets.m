function rowSets = findRowSets(A, bucketSizeThresh, delta, row1, row2)
MAX_RATIO_VAL = 10;
MIN_RATIO_VAL = -10;
maxNumberOfBuckets = 1;
[~, m] = size(A);
rowBuckets = zeros(0, m);

ratios = A(row1, :) ./ A(row2, :);
ratios = log(ratios);
if nnz(~isnan(ratios) .* ~isinf(ratios)) >= 5
    idx = ~isinf(ratios);
    mx = max(ratios(idx));
    mn = min(ratios(idx));
    nBuckets = ceil((mx-mn)/delta);
%     buckets = zeros(nBuckets, m);
    buckets = sparse(nBuckets, m);
    for colIdx = 1 : m
        temp = ratios(colIdx);
        if ~isnan(temp) && ~isinf(temp) && temp <= MAX_RATIO_VAL && temp >= MIN_RATIO_VAL
            bucketId = ceil((temp-mn)/delta + 1e-6);
            buckets(bucketId, colIdx) = 1;
        end
    end
    
    bucketSizes = sum(buckets, 2);
    bucketsToDelete = bucketSizes < bucketSizeThresh;
    
    buckets(bucketsToDelete, :) = [];
    rowBuckets = [rowBuckets; buckets];
    
%     rowBuckets = unique(rowBuckets, 'rows');
    
    rowSetCount = 0;
    for bucketIdx = 1 : size(rowBuckets, 1)
        rowSetCount = rowSetCount + 1;
        rowSets{rowSetCount} = sort(find(rowBuckets(bucketIdx, :)));
    end
    
    if ~exist('rowSets') % No correlated rows were found.
        rowSets = {};
    end
    
    %% Use only largest buckets
    nBlocks = numel(rowSets);
    if nBlocks > maxNumberOfBuckets
        sizes = nan(nBlocks, 1);
        for i = 1 : nBlocks
            sizes(i) = numel(rowSets{i});
        end
        [~, idx] = sort(sizes, 'descend');
        idx = idx(1:maxNumberOfBuckets);
        rowSets = rowSets(idx);
    end
else
    rowSets = {};
end

end