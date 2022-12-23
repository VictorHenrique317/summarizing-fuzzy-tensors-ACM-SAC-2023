function val = L1(A, B, C, algebra)

if algebra == 0  % max-times
    Result = MaxProduct(B, C);
else             % max-plus
    Result = MaxSum(B, C);
end
val = nansum(nansum(abs(A - Result))) / nansum(nansum(abs(A)));

end