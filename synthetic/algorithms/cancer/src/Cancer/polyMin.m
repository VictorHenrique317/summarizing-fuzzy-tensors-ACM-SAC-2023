function [objBest, alpha] = polyMin(p, a, a_, b, ALGEBRA)

ALLOW_NEGATIVES = 0;
if ALGEBRA == 0
    ALLOW_NEGATIVES = 1;
end

p = fliplr(p);
d = polyder_(p);
r = real(roots(d));
alpha = 0;
objBest = maxObj(alpha, a, a_, b, ALGEBRA);
for i = 1 : numel(r)
    if r(i) > 0 || ALLOW_NEGATIVES == 1
        newVal = maxObj(r(i), a, a_, b, ALGEBRA);
        if newVal < objBest
            objBest = newVal;
            alpha = r(i);
        end
    end
end


% alpha_ = 0:0.01:10;
% val = nan(numel(alpha_), 1);
% for i = 1 : numel(alpha_)
%     val(i) = J(alpha_(i), a, a_, b, ALGEBRA);
%     %debug
%     val2(i) = J2(alpha_(i), a, a_, b, ALGEBRA);
%     %debug
% end
% mn = ones(size(val)) * min(val);
% 
% % valRelax = nan(numel(alpha), 1);
% % for i = 1 : numel(alpha)
% %     valRelax(i) = Jrel(alpha(i), a, a_, b);
% % end
% % [~, idx] = min(valRelax);
% % mnRelax = ones(size(val)) * val(idx);
% 
% 
% for i = 1 : numel(alpha_)
%     valRelax(i) = polyMaxObj(alpha_(i), fliplr(p));
% end
% [~, idx] = min(valRelax);
% mnRelax = ones(size(val)) * val(idx);
% 
% 
% %% Plot.
% plot(alpha_, val, 'color', 'b');
% hold on
% plot(alpha_, mn, 'color', 'b');
% 
% plot(alpha_, valRelax, 'color', 'r');
% plot(alpha_, mnRelax, 'color', 'r');
% hold off

end












function val = J(alpha, a, a_, b, ALGEBRA)
if ALGEBRA == 0
    val = norm(a - max(alpha*b, a_), 'fro')^2;
else
    val = norm(a - max(alpha + b, a_), 'fro')^2;
end
end


function val = J2(alpha, a, a_, b, ALGEBRA)
if ALGEBRA == 0
    val = norm(a - max(alpha*b, a_), 'fro');
else
    val = norm(a - max(alpha + b, a_), 'fro');
end
end


function val = Jrel(alpha, a, a_, b)
val = norm(a - maxRelaxed(alpha*b, a_), 'fro')^2;
end


function val = maxRelaxed(a, b)
n = numel(a);
val = nan(n, 1);
for i = 1 : n
    val(i) = (relaxFn(a(i))*a(i) + relaxFn(b(i))*b(i)) / (relaxFn(a(i)) + relaxFn(b(i)));
end
end


function val = relaxFn(x)
sigma = 6;
% val = exp(sigma * x);
val = x^sigma;
end


function val = polyMaxObj(alpha, p)
n = numel(p);
val = 0;
temp = 1;
for i = 1 : n
    val = val + p(i) * temp;
    temp = temp * alpha;
end
end


function p = getPolyMaxObj(a, a_, b)
n = 4;
x = 0 : 1/n : 1;
y = nan(size(x));
for i = 1 : n+1
    y(i) = J(x(i), a, a_, b);
end
q = polyfit(x, y, n);
p = fliplr(q);
end