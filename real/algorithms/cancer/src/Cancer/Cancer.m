function [B, C, varargout] = Cancer(A, k, varargin)
NCYCLES = 14;
for i = 1 : numel(varargin)
    if strcmp(varargin{i}, 'NCYCLES')
        NCYCLES = varargin{i + 1};
    end
end


%% Initial factors

% [B, C] = C8(A, k);

[n, m] = size(A);
B = zeros(n, k);
C = zeros(k, m);


%% Postprocessing with polynomial optimization.
if nargout > 2
    [B, C, varargout{1}] = PostCancer(A, B, C, 'NCYCLES', NCYCLES);
else
    [B, C] = PostCancer(A, B, C, 'NCYCLES', NCYCLES);
end

end
