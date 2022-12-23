function [ bestA, bestX, bestErr, bestErrs ] = nmf_als_l1( Y, k, varargin )
%NMF_ALS_L1 L1-reguliarized NMF via ALS
%   [W, H] = NMF_ALS_L1(A, k) for the unregularized ALS NMF
%   [W, H] = NMF_ALS_L1(A, k, a) uses a as the regularizes weight for both
%   factor matrices
%   [W, H] = NMF_ALS_L1(A, k, a, b) uses a for W and b for H
%   [W, H] = NMF_ALS_L1(..., r) runs the algorithm r times and returns the
%   best solution (default = 10)
%
%   See also: NNMF

maxIters   = 300;
minImprove = 0;

p = inputParser;

addRequired(p, 'Y', @(x) validateattributes(x, {'numeric'}, {'2d', 'nonnegative'}, mfilename, 'A', 1));
addRequired(p, 'k', @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer', 'positive'}, mfilename, 'k', 2));
addOptional(p, 'a', 0, @(x) validateattributes(x, {'numeric'}, {'scalar', 'nonnegative'}, mfilename, 'a', 3));
addOptional(p, 'b', nan, @(x) validateattributes(x, {'numeric'}, {'scalar', 'nonnegative'}, mfilename, 'b', 4));
addOptional(p, 'r', 10, @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer', 'positive'}, mfilename, 'r', 5));

parse(p, Y, k, varargin{:});

a = p.Results.a;
b = p.Results.b;
if isnan(b), b = a; end
r = p.Results.r;


[n, m] = size(Y);

bestErr = Inf;
bestA = zeros(n, k);
bestX = zeros(k, m);
bestErrs = nan(1, maxIters+1);

for rep=1:r
    X = rand(k, m);

    errs = nan(1, maxIters+1);
    errs(1) = norm(Y, 'fro');
    
    for iter = 1:maxIters
        A = (Y*X' - a*ones(n,k))*pinv(X*X');
        A(A<0) = 0;
        X = pinv(A'*A)*(A'*Y - b*ones(k,m));
        X(X<0) = 0;
        errs(iter+1) = norm(Y - A*X, 'fro');        
        if iter > 2 && errs(iter) - errs(iter+1) < minImprove*norm(Y, 'fro')
            disp('Converged!')
            break;
        end
    end
    if min(errs) < bestErr
        bestErr = min(errs);
        bestErrs = errs;
        bestA = A;
        bestX = X;
    end
end    
end

