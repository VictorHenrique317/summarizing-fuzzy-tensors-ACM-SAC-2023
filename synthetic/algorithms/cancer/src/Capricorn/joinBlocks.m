function [b, c] = joinBlocks(b1, c1, b2, c2)
ratioB = b1 ./ b2;
ratioC = c1 ./ c2;
idx = isinf(ratioB);
ratioB(idx) = nan;
idx = isinf(ratioC);
ratioC(idx) = nan;

stdB = nanstd(ratioB);
stdC = nanstd(ratioC);

if stdC < stdB
    [b, c] = joinAlongC(b1, c1, b2, c2);
else
    [s, t] = joinAlongC(c1', b1', c2', b2');
    b = t';
    c = s';
end
end


function [b, c] = joinAlongC(b1, c1, b2, c2)
ratios = c2 ./ c1;
idx = isinf(ratios);
ratios(idx) = nan;
alpha = nanmean(ratios);
b2 = alpha * b2;
b = max(b1, b2);
c = c1;
end