function newX = blockAnnealFun(optimValues, problemData,  prob0To1, prob1To0)
newX = optimValues.x;
nVars = numel(newX);
t = max(optimValues.temperature);
initT = 100;
if t < 5
    nVarsChange = 1;
else
    nVarsChange = min(nVars, ceil(nVars/3 * log(t+1)/log(initT)));
end
idx = find(rand(size(newX)) < nVarsChange/numel(newX));
for i = 1 : numel(idx)
    if newX(idx(i)) == 0
        if rand() < prob0To1(idx(i))
            newX(idx(i)) = 1;
        end
    else
        if rand() < prob1To0(idx(i))
            newX(idx(i)) = 0;
        end
    end
end
end