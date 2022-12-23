function [Bbest, Cbest] = NMFCancer(A, k)
profile on
N_CYCLES = 2;
MAX_ITER = N_CYCLES * k;

%% Run NMF
[B ,C] = nmf(A, k);

%% Adjust factors using polynomial approximation.
Bbest = B;
Cbest = C;
errorBest = errorTrop(A, B, C);
% Cyclic updates.
for iter = 1 : MAX_ITER
    blockIdx = rem(iter, k) + 1;
    B_ = B;
    C_ = C;
    B_(:, blockIdx) = [];
    C_(blockIdx, :) = [];
    A_ = MaxProduct(B_, C_);
    [b, c] = rank1C8(A, A_, B(:, blockIdx), C(blockIdx, :));
    B(:, blockIdx) = b;
    C(blockIdx, :) = c;
    newError = errorTrop(A, B, C);
    if newError < errorBest
        errorBest = newError;
        Bbest = B;
        Cbest = C;
    end
    
    % Print error.
    Acurrent = MaxProduct(B, C);
    idxUnder = double(Acurrent < A);
    errUnder = norm(idxUnder .* (A - Acurrent), 'fro') / norm(A, 'fro');
    idxOver = double(Acurrent > A);
    errOver = norm(idxOver .* (A - Acurrent), 'fro') / norm(A, 'fro');
    fprintf('iter: %d   rank: %d   error: %f   under: %f   over: %f   best error: %f\n', ...
        iter, size(B, 2), newError, errUnder, errOver, errorBest);
end
profile viewer
end

function err_ = errorTrop(A, B, C)
err_ = norm(A - MaxProduct(B, C), 'fro') / norm(A, 'fro');
end