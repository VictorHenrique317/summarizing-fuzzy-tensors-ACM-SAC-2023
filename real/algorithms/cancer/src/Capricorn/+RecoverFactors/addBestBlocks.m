function [B, C] = addBestBlocks(A, rows, cols)
[n, m] = size(A);
B = zeros(n, 0);
C = zeros(0, m);
currentScore = L1(A, B, C, 0);
usedBlocks = [];
delta = -1;
while delta < -0.01 
    newScore = 1e10;
    bestIdx = -1;
    for i = 1 : numel(rows)
        if sum(usedBlocks == i) == 0
            t = L1(A, [B, rows{i}], [C; cols{i}], 0);
            if t < newScore
                newScore = t;
                bestIdx = i;
            end
        end
    end
    delta = newScore - currentScore;
    currentScore = newScore;
    if delta >= 0
        break;
    end
    B = [B, rows{bestIdx}];
    C = [C; cols{bestIdx}];
    [B, C] = squeezeFactor(B, C);
    [C, B] = squeezeFactor(C', B');
    B = B';
    C = C';
    usedBlocks = [usedBlocks, bestIdx];
end

end