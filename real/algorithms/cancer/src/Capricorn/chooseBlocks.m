function blocks = chooseBlocks(A, rows, cols)
initT = 100;
MaxIter = 12000;
epsilon = 1e-5;
verbose = 1;
MAX_BLOCKS = 150;
[n, m] = size(A);
nblocks = min(numel(rows), MAX_BLOCKS);

% x0 = zeros(nblocks, 1);
x0 = rand(nblocks, 1) < 0.5;
obj = @(x) blockSelectionScore(x, A, rows, cols);

prob0To1 = nan(nblocks, 1);
for i = 1 : nblocks
    prob0To1(i) = (nblocks - i)/2;    
end
prob0To1 = prob0To1/nblocks;
prob1To0 = 1 - prob0To1;

options = saoptimset('DataType', 'custom', 'AnnealingFcn', ...
    @(optimValues, problemData)blockAnnealFun(optimValues, problemData, prob0To1, prob1To0), 'MaxIter', MaxIter, ...
    'ObjectiveLimit', epsilon, 'InitialTemperature', initT, 'TemperatureFcn', ...
    @tempFcn);
if verbose == 1
    options = saoptimset(options, 'PlotInterval',10, ...
        'PlotFcns',{@saplotbestf,@saplottemperature,@saplotf,@saplotstopping});
end

[blocks, fval, exitFlag, output] = simulannealbnd(obj, x0, 1, 1, options);

end