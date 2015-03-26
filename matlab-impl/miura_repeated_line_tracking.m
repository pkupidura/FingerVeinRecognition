function veins = miura_repeated_line_tracking(img, fvr, iterations, r, W)
% Repeated line tracking

% Parameters:
%  img        - Input vascular image
%  fvr        - Binary image of the finger region
%  iterations - Maximum number of iterations
%  r - Distance between tracking point and cross section of the profile
%  W - Width of profile

% Returns:
%  veins - Vein image

% Reference:
% Feature extraction of finger vein patterns based on repeated line 
%    tracking and its application to personal identification
% N. Miura, A. Nagasaka, and T. Miyatake
% Machine Vision and Applications, Volume 15, Number 4 (2004), pp. 194--203
% doi: 10.1007/s00138-004-0149-2

% Author:  Bram Ton <b.t.ton@alumnus.utwente.nl>
% Date:    20th December 2011
% License: Simplified BSD License

p_lr = 0.5;  % Probability of goin left or right
p_ud = 0.25; % Probability of going up or down
% writerObj = VideoWriter('peaks.avi');
% open(writerObj);
Tr = zeros(size(img));     % Locus space
bla = [-1,-1; -1,0; -1,1; 0,-1; 0,0; 0,1; 1,-1; 1,0; 1,1];

% Check if W is even
if (mod(W,2) == 0)
    disp('Error: W must be odd')
end
ro = round(r*sqrt(2)/2);   % r for oblique directions
hW = (W-1)/2;              % half width for horz. and vert. directions
hWo = round(hW*sqrt(2)/2); % half width for oblique directions

% Omit unreachable borders
fvr(1:r+hW,:) = 0;
fvr(end-(r+hW-1):end,:) = 0;
fvr(:,1:r+hW) = 0;
fvr(:,end-(r+hW-1):end) = 0;

%% Uniformly distributed starting points
indices = find(fvr > 0);
a = randperm(length(indices));
a = a(1:iterations); % Limit to number of iterations
[ys, xs] = ind2sub(size(img), indices(a));
%ys = [200;20];% LET OP
%xs=  [200;200];% LET OP

%% Iterate through all starting points
for it = 1:size(ys,1)
    xc = xs(it); % Current tracking point, x
    yc = ys(it); % Current tracking point, y
    
    % Determine the moving-direction attributes
    % Going left or right ?
    if rand() >= 0.5
        Dlr = -1;  % Going left
    else
        Dlr = 1; % Going right 
    end

    % Going up or down ?
    if rand() >= 0.5
        Dud = -1;  % Going up
    else
        Dud = 1; % Going down 
    end
    
    % Initialize locus-positition table Tc
    Tc = false(size(img));

    %Dlr = -1; Dud=-1;% LET OP
    Vl = 1;
    while Vl > 0;
        %% Determine the moving candidate point set Nc
        Nr = false(3);
        Rnd = rand();
        %Rnd = 0.8;% LET OP
        if Rnd < p_lr
            % Going left or right
            Nr(:,2+Dlr) = true;
        elseif (Rnd >= p_lr) && (Rnd < p_lr + p_ud)  
            % Going up or down
            Nr(2+Dud,:) = true;
        else
            % Going any direction
            Nr = true(3);
            Nr(2,2) = false;
        end

        tmp = find( ~Tc(yc-1:yc+1,xc-1:xc+1) & Nr & fvr(yc-1:yc+1,xc-1:xc+1) );
        Nc =[xc + bla(tmp,1), yc + bla(tmp,2)];
        
        if size(Nc,1)==0
            Vl=-1;
            continue
        end

        %% Detect dark line direction near current tracking point
        Vdepths = zeros(size(Nc,1),1); % Valley depths
        for i = 1:size(Nc,1)
            % Horizontal or vertical 
            if Nc(i,2) == yc
                % Horizontal plane
                yp = Nc(i,2);
                if Nc(i,1) > xc
                    % Right direction
                    xp = Nc(i,1) + r;
                else
                    % Left direction
                    xp = Nc(i,1) - r;
                end

                Vdepths(i) = img(yp + hW, xp) - ...
                    2*img(yp,xp) + ...
                    img(yp - hW, xp);
            elseif Nc(i,1) == xc
                % Vertical plane
                xp = Nc(i,1);
                if Nc(i,2) > yc
                    % Down direction
                    yp = Nc(i,2) + r;
                else
                    % Up direction
                    yp = Nc(i,2) - r;
                end

                Vdepths(i) = img(yp, xp + hW) - ...
                    2*img(yp,xp) + ...
                    img(yp, xp - hW);
            end
            
            % Oblique directions
            if ((Nc(i,1) > xc) && (Nc(i,2) < yc)) || ((Nc(i,1) < xc) && (Nc(i,2) > yc))
                % Diagonal, up /
                if Nc(i,1) > xc && Nc(i,2) < yc
                    % Top right
                    xp = Nc(i,1) + ro;
                    yp = Nc(i,2) - ro;
                else
                    % Bottom left
                    xp = Nc(i,1) - ro;
                    yp = Nc(i,2) + ro;
                end

                Vdepths(i) = img(yp - hWo, xp - hWo) - ...
                    2*img(yp,xp) + ...
                    img(yp + hWo, xp + hWo);
            else
                % Diagonal, down \
                if Nc(i,1) < xc && Nc(i,2) < yc
                    % Top left
                    xp = Nc(i,1) - ro;
                    yp = Nc(i,2) - ro;
                else
                    % Bottom right
                    xp = Nc(i,1) + ro;
                    yp = Nc(i,2) + ro;
                end
                
                Vdepths(i) = img(yp + hWo, xp - hWo) - ...
                    2*img(yp,xp) + ...
                    img(yp - hWo, xp + hWo);
            end
        end %  End search of candidates

        [~, index] = max(Vdepths); %  Determine best candidate

        % Register tracking information
        Tc(yc, xc) = true;

        % Increase value of tracking space
        Tr(yc, xc) = Tr(yc, xc) + 1;
        %writeVideo(writerObj,Tr);
        
        % Move tracking point
        xc = Nc(index, 1);
        yc = Nc(index, 2); 
    end
end
veins = Tr;