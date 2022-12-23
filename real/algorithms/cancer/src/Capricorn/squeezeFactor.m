function [B, C] = squeezeFactor(B, C, tolerance)
k = size(B, 2);
if k > 1
    idx = [];
    for i = 1 : k-1        
        if nnz(B(:, end) ~= B(:, i)) <= tolerance
            idx = [idx, i];
        end
    end
    if numel(idx) ~= 0
        C_ = C(idx, :);
        c = max(C_, [], 1);
        c = max(c, C(end, :));
        C(idx, :) = [];
        B(:, idx) = [];
        C(end, :) = c;
    end
end

end