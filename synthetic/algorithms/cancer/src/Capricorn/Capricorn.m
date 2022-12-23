function [Bbest, Cbest] = Capricorn(A, k, varargin)
% Finds max-times (subtropical) decomposition of A of rank k.
% Usage:
% [B, C] = Capricorn(A, k, opt1, val1, opt2, val2, ...)
% A - nonnegative real matrix
% k - rank (positive integer)
% B - first factor (nonnegative real)
% C - second factor (nonnegative real)
% Optional parameters:
% MAX_CYCLES         - defines the number of cycles used for cyclic updates.
% eps_               - if we obtain an approximation of A with L1 error < eps_, 
%                      the algorithm exist
% bucketSizeThresh   - minimum number of elements in a bucket
% overfitTolerance   - when considering adding a row to a block, 
%                      overfitTolerance defines the maximum ratio of 
%                      negative to positive error changes that we accept
% delta              - bucket width
% precisionThreshold - if the L1 norm of the residual drops below 
%                      precisionThreshold * (L1 norm of A), we stop

MAX_CYCLES = 4;
eps_ = 1e-2;
bucketSizeThresh = 3;
overfitTolerance = 0.5;
% delta = 0.001;
delta = 0.01;
precisionThreshold = 0.001;


Orig = A;
Aplot = Orig;

for i = 1 : nargin - 3
    if isequal(varargin{i}, 'MAX_CYCLES')
        MAX_CYCLES = varargin{i+1};
    end
    
    if isequal(varargin{i}, 'eps_')
        eps_ = varargin{i+1};
    end
    
    if isequal(varargin{i}, 'bucketSizeThresh')
        bucketSizeThresh = varargin{i+1};
    end
    
    if isequal(varargin{i}, 'overfitTolerance')
        overfitTolerance = varargin{i+1};
    end
    
    if isequal(varargin{i}, 'delta')
        delta = varargin{i+1};
    end
    
    if isequal(varargin{i}, 'precisionThreshold')
        precisionThreshold = varargin{i+1};
    end
    
    if isequal(varargin{1}, 'Aplot')
        Aplot = varargin{i+1};
    end
end

weight = nansum(nansum(A));
[n, m] = size(A);
B = zeros(n, 0);
C = zeros(0, m);
iter = 0;
best_error = 1e8;
Bbest = zeros(n, 0);
Cbest = zeros(0, m);
max_beta = 0.95;
beta = max_beta;
min_beta = 0.75;
sigma = 0.05;
while nansum(nansum(A)) >= precisionThreshold*weight
    iter = iter + 1;
    sm = nansum(A, 2);
    [~, idx] = max(sm);
    Corrs_ = correlationsWithRow(A, idx, bucketSizeThresh, delta);
    [bidx, cidx] = getPattern(Corrs_);
    if isempty(bidx) || isempty(cidx)
        b = zeros(n, 1);
        c = zeros(1, m);
    else
        [b, c] = recoverBlock(A, bidx, cidx, Corrs_);
        [b, c] = expandBlock(b, c, A, Orig, bucketSizeThresh, ...
            overfitTolerance, delta);
    end
    
    B = [B, b];
    C = [C; c];
    
    if iter >= 2 * k
        [B, C] = tryBlockJoins(Orig, B, C, beta);
        if rem(iter-1, k) == 0
            beta = beta - sigma;
            if beta < min_beta
                beta = max_beta;
            end
        end
    end
    
    error_ = L1(Orig, B, C, 0);
    errorPrint = L1(Aplot, B, C, 0);
    fprintf('[Capricorn]: ITER %d   error: %d\n', iter, errorPrint);
    if error_ < best_error
        best_error = error_;
        Bbest = B;
        Cbest = C;
    end
    idx = find(b*c > A-eps_);
    A(idx) = nan;
    
    if iter > MAX_CYCLES * k
        break;
    end
    
    if size(B, 2) >= k
        B(:, 1) = [];
        C(1, :) = [];
        A_ = MaxProduct(B, C);
        A = Orig;
        idx = find(A_ > Orig-eps_);
        A(idx) = nan;
    end
end

end