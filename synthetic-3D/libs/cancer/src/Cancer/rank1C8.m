function [b, c] = rank1C8(A, A_, binit, cinit, degree, ALGEBRA)
DISALLOW_BIG_VALS = 1;
MAX_VAL = 5;
MIN_VAL = 0;

%% Initialize
[n, m] = size(A);
MAX_ITER = floor((n+m)/20);
% b = zeros(n, 1);
% c = zeros(1, m);
b = binit;
c = cinit;
sm = sum(A - A_, 2);
[~, idx] = max(sm);
b(idx) = max(A(idx, :));
% idx = randi(n);

%% Alternating updates
count = 1;
while count <= MAX_ITER
    if rem(count, 2) == 1
        [idx, newVal] = adjustOneElement(A, b, c, A_, degree, ALGEBRA);
        if idx == -1
            return;
        end
        c(idx) = newVal;
    else
        [idx, newVal] = adjustOneElement(A', c', b', A_', degree, ALGEBRA);
        if idx == -1
            return;
        end
        b(idx) = newVal;
    end
    count = count + 1;
end

if DISALLOW_BIG_VALS == 1
    b = min(b, MAX_VAL);
    c = min(c, MAX_VAL);
    b = max(b, MIN_VAL);
    c = max(c, MIN_VAL);
end

end

