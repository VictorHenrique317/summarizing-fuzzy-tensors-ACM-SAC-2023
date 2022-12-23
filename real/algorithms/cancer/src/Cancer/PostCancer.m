function [Bbest, Cbest, varargout] = PostCancer(A, B, C, varargin)
ALGEBRA = 0;  % 0 - max-times,  1 - max-plus
k = size(B, 2);
NCYCLES = 3;
printOriginal = 0;
for i = 1 : numel(varargin)
    if strcmp(varargin{i}, 'original')
        printOriginal = 1;
        Aoriginal = varargin{i + 1};
    end
    if strcmp(varargin{i}, 'NCYCLES')
        NCYCLES = varargin{i + 1};
    end
end
MAX_ITER = NCYCLES * k - 1;

%% Adjust factors using polynomial approximation.
degree = 2;
Bbest = B;
Cbest = C;
errorBest = errorTrop(A, B, C, ALGEBRA);
errors = nan(MAX_ITER + 1, 1);
% Cyclic updates.
for iter = 0 : MAX_ITER
    blockIdx = rem(iter, k) + 1;
    B_ = B;
    C_ = C;
    B_(:, blockIdx) = [];
    C_(blockIdx, :) = [];
    if ALGEBRA == 0
        A_ = MaxProduct(B_, C_);
    else
        A_ = MaxSum(B_, C_);
    end
    [b, c] = rank1C8(A, A_, B(:, blockIdx), C(blockIdx, :), degree, ALGEBRA);
    B(:, blockIdx) = b;
    C(blockIdx, :) = c;
    
    newError = errorTrop(A, B, C, ALGEBRA);
    errors(iter+1) = newError;
    if newError < errorBest
        errorBest = newError;
        Bbest = B;
        Cbest = C;
    end
  
    
    % Print error.
    Acurrent = MaxProduct(B, C);
    idxUnder = double(Acurrent < A);
    errUnder = errorTrop(A, B, C, ALGEBRA, idxUnder);
    idxOver = double(Acurrent > A);
    errOver = errorTrop(A, B, C, ALGEBRA, idxOver);
    if printOriginal == 1
        trueError = errorTrop(Aoriginal, B, C, ALGEBRA);
        fprintf('iter: %d   rank: %d   error: %f   under: %f   over: %f   best error   %f:   true error: %f\n', ...
            iter, size(B, 2), newError, errUnder, errOver, errorBest, trueError);
    else
        fprintf('iter: %d   rank: %d   error: %f   under: %f   over: %f   best error: %f\n', ...
            iter, size(B, 2), newError, errUnder, errOver, errorBest);
    end
    
    
    %% Gradualy increase the degree.
    if iter > k && rem(iter, k) == 0
        degree = degree + 1
    end
    if degree > 16
        degree = 2;
    end
    
    
    %% Alternate high and low degrees.
    % if iter > k
    %     if rem(iter, k) == 0
    %         degree = degree + 1
    %     end
    %     if degree > 30
    %         degree = 2;
    %     end
    % end
end
if nargout > 2
    varargout{1} = errors;
end
end

function err_ = errorTrop(A, B, C, ALGEBRA, varargin)
if ~isempty(varargin)
    W = varargin{1};
else
    W = ones(size(A));
end

if ALGEBRA == 0
    % here
    err_ = norm((A - MaxProduct(B, C)) .* W, 'fro') / norm(A, 'fro');
else
    err_ = norm((A - MaxSum(B, C)) .* W, 'fro') / norm(A, 'fro');
end
end