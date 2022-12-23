function output = MaxSum(B, W)
n = size(B, 1);
m = size(W, 2);

if numel(B)*numel(W) ~= 0  % nonempty factors
    output = zeros(n, m);
    for i = 1:n
        output(i, :) = max(bsxfun(@plus, B(i, :)', W));
    end
else
    output = zeros(n, m);
end

end