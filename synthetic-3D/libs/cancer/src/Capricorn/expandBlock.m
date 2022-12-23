function [b, c] = expandBlock(b, c, R, Original, bucketSizeThresh, overfitTolerance, delta)

b_ = addRows(b, c, Original, R, bucketSizeThresh, overfitTolerance, delta);
c_ = addRows(c', b', Original', R', bucketSizeThresh, overfitTolerance, delta)';
b = b_;
c = c_;

end


function b = addRows(b, c, Original, R, bucketSizeThresh, overfitTolerance, delta)
[n, m] = size(Original);
bIdx = find(b > 0);
t = setdiff(1:n, bIdx);

residualOption = 2;

switch residualOption
    case 1
        posH = Original;
        negH = Original;
    case 2
        posH = R;
        negH = Original;
    case 3
        posH = Original;
        negH = R;
    case 4
        posH = R;
        negH = R;
end

for i = 1 : numel(t)
    idx = t(i);
    % Compute correlations.
    temp = nan(2, m);
    temp(1, :) = c;
    temp(2, :) = Original(idx, :);
    rowSets = findRowSets(temp, bucketSizeThresh, delta, 1, 2);
    if numel(rowSets) == 0
        continue;
    end
    intersectingSet = rowSets{1};
    % Check if we overfit.
    alpha = nanmean(Original(idx, intersectingSet) ./ c(intersectingSet));
    newRow = alpha*c;
    positiveImpact = nansum(posH(idx, :) - abs(posH(idx, :) - newRow));
    negIdx = find(newRow > negH(idx, :));
    negativeImpact = sum(newRow(negIdx) - negH(idx, negIdx));
    
    
    if positiveImpact <= 0 || negativeImpact / positiveImpact > overfitTolerance
        continue;
    end
    b(idx) = alpha;
    
end
end