function [Bnew, Cnew] = tryBlockJoins(A, B, C, beta)
Bnew = B;
Cnew = C;
k = size(B, 2);
if k >= 2
    beta = 0.7;
    algebra = 0; % Max-times.
    errorOriginal = L1(A, B, C, algebra);
    bestImpact = 0;
    for i = 1 : k-1
        [b, c] = joinBlocks(B(:, k), C(k, :), B(:, i), C(i, :));
        B_ = B;
        C_ = C;
        idx = [i, k];
        B_(:, idx) = [];
        C_(idx, :) = [];
        impactOriginal = L1(A, B_, C_, algebra) - errorOriginal;
        B__ = [B_, b];
        C__ = [C_; c];
        impactNew = L1(A, B_, C_, algebra) - L1(A, B__, C__, algebra);
        if impactNew < 0
            continue;
        end
        if impactNew/impactOriginal < beta
            continue;
        end
        if impactNew > bestImpact
            bestImpact = impactNew;
            Bnew = B__;
            Cnew = C__;
        end
    end
end

end