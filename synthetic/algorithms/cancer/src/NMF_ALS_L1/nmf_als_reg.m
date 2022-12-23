function [B, C] = nmf_als_reg(A, k)
regularizer = 1;
nrestarts = 10;
[B, C] = nmf_als_l1(A, k, 'a', regularizer, 'r', nrestarts);

end

