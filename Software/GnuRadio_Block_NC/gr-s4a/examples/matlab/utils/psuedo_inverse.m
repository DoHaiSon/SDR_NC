% Computes Psuedo-Inverse of a non-square matrix using SVD
%
% Ref: http://www.koders.com/cpp/fidB51C746BC84839854FEE91B738EBD512BFBCB88E.aspx?s=psuedo-inverse
A = [(1.0000+1i*(0.0000))  (5.0000+1i*(0.0000))  ;(2.0000+1i*(0.0000))  (6.0000+1i*(0.0000))  ;(3.0000+1i*(0.0000))  (7.0000+1i*(0.0000))  ;(4.0000+1i*(0.0000))  (8.0000+1i*(0.0000))  ;];
rows = size(A,1);
columns = size(A,2);
[U,S,V] = svd(A);
SM = zeros(columns,rows);

TOL = S(1)*max(columns,rows)*eps;

S_sz = min(size(S,1),size(S,2));

SM = zeros(size(A.'));
for idx=1:S_sz
   SM_val = 0;
   if abs(S(idx,idx)) > TOL
       SM_val = 1/S(idx,idx); 
   else
       SM_val = 0;
   end
   SM(idx,idx) = SM_val;  
end

R_est = V*SM*(U.')

R = pinv(A);

if sum(sum(R - R_est)) == 0
   disp('Succeed :)'); 
else
    disp('Failed :(');
end