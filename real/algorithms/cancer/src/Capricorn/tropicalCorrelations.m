function Q = tropicalCorrelations(A, rowIdx)
% function [Qrow, Qcol] = tropicalCorrelations(A)
[n, m] = size(A);
Q = zeros(n, m);
for i = 1 : n
    for j = 1 : m
        if A(rowIdx, j) >= A(i, j)
            Q(i, j) = 1;           
        end
    end
end

% Qrow = nan(n, n, m);
% for i1 = 1 : n
%     for i2 = 1 : n
%         for j = 1 : m
%             if i1 > i2
%                 if A(i1, j) >= A(i2, j)
%                     Qrow(i1, i2, j) = 1;
%                     Qrow(i2, i1, j) = 0;
%                 else
%                     Qrow(i1, i2, j) = 0;
%                     Qrow(i2, i1, j) = 1;
%                 end
%             end
%         end
%     end
% end
% for i = 1 : n
%     Qrow(i, i, :) = 1;
% end
%
% Qcol = nan(m, m, n);
% for j1 = 1 : m
%     for j2 = 1 : m
%         for i = 1 : n
%             if j1 > j2
%                 if A(i, j1) >= A(i, j2)
%                     Qcol(j1, j2, i) = 1;
%                     Qcol(j2, j1, i) = 0;
%                 else
%                     Qcol(j1, j2, i) = 0;
%                     Qcol(j2, j1, i) = 1;
%                 end
%             end
%         end
%     end
% end
% for j = 1 : m
%     Qcol(j, j, :) = 1;
% end

end