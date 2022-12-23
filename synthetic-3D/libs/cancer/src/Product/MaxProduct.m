function output = MaxProduct(B, W)
n = size(B, 1);
m = size(W, 2);

if size(B, 2) == 0
    output = zeros(n, m);
elseif size(B, 2) == 1
    output = B * W;
else
    output = zeros(n, m);
    for i = 1:n
        output(i, :) = max(bsxfun(@times, B(i, :)', W));
    end
end

end