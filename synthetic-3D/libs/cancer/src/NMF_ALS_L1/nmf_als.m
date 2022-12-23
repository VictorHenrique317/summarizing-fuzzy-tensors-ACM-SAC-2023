function [B, C] = nmf_als(A, k)
regularizer = 0;
nrestarts = 10;
[B, C] = nmf_als_l1(A, k, 'a', regularizer, 'r', nrestarts);

end

