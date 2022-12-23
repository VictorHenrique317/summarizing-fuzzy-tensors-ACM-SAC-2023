function [B, C] = ALSR5(A, k)
regularizer = 5;
nrestarts = 10;
[B, C] = nmf_als_l1(A, k, 'a', regularizer, 'r', nrestarts);

end

