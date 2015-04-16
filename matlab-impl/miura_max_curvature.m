function veins = miura_max_curvature(img, fvr, sigma)
% Maximum curvature method

% Parameters:
%  img    - Input vascular image
%  fvr    - Finger vein region
%  sigma  - Sigma used for determining derivatives

% Returns:
%  veins - Vein image

% Reference:
% Extraction of finger-vein patterns using maximum curvature points in
%   image profiles
% N. Miura, A. Nagasaka, and T. Miyatake
% IAPR conference on machine vision applications 9 (2005), pp. 347--350  

% Author:  Bram Ton <b.t.ton@alumnus.utwente.nl>
% Date:    20th December 2011
% License: Simplified BSD License

% Changelog:
% 2012/01/10 - Speed enhancement by rewriting diag() functions
%              with linear indices.   

% Construct filter kernels
winsize = ceil(4*sigma);
[X,Y] = meshgrid(-winsize:winsize, -winsize:winsize);

h = (1/(2*pi*sigma^2)).*exp(-(X.^2 + Y.^2)/(2*sigma^2));
hx = (-X/(sigma^2)).*h;
hxx = ((X.^2 - sigma^2)/(sigma^4)).*h;
hy = hx';
hyy = hxx';
hxy = ((X.*Y)/(sigma^4)).*h;

% Do the actual filtering
fx  = imfilter(img, hx,  'replicate', 'conv');
fxx = imfilter(img, hxx, 'replicate', 'conv');
fy  = imfilter(img, hy,  'replicate', 'conv');
fyy = imfilter(img, hyy, 'replicate', 'conv');
fxy = imfilter(img, hxy, 'replicate', 'conv');
f1  = 0.5*sqrt(2)*(fx + fy); % \
f2  = 0.5*sqrt(2)*(fx - fy); % /
f11 = 0.5*fxx + fxy + 0.5*fyy; % \\
f22 = 0.5*fxx - fxy + 0.5*fyy; % //
[img_h, img_w] = size(img); % Image height and width

%% Calculate curvatures
k = zeros(img_h, img_w, 4);
k(:,:,1) = (fxx./((1 + fx.^2).^(3/2))).*fvr; % hor
k(:,:,2) = (fyy./((1 + fy.^2).^(3/2))).*fvr; % ver
k(:,:,3) = (f11./((1 + f1.^2).^(3/2))).*fvr; % \
k(:,:,4) = (f22./((1 + f2.^2).^(3/2))).*fvr; % / 

%% Scores
%V = zeros(img_h, img_w, 4);
Vt = zeros(img_h, img_w);
Wr = 0;

% Horizontal direction
bla = k(:,:,1) > 0;
for y=1:img_h
    for x=1:img_w
        if(bla(y,x))
            Wr = Wr + 1;
        end
        
        if ( Wr > 0 && (x == img_w || ~bla(y,x)) )
            if (x == img_w)
                % Reached edge of image
                pos_end = x;
            else
                pos_end = x - 1;              
            end
            
            pos_start = pos_end - Wr + 1; % Start pos of concave      
            [~, I] = max(k(y, pos_start:pos_end,1));
            pos_max = pos_start + I - 1;
            Scr = k(y,pos_max,1)*Wr;
            %V(y,pos_max,1) = V(y,pos_max,1) + Scr;
            Vt(y,pos_max) = Vt(y,pos_max) + Scr;
            Wr = 0; 
        end
    end
end

% Vertical direction
bla = k(:,:,2) > 0;
for x=1:img_w
    for y=1:img_h
        if(bla(y,x))
            Wr = Wr + 1;
        end
        
        if ( Wr > 0 && (y == img_h || ~bla(y,x)) )
            if (x == img_h)
                % Reached edge of image
                pos_end = y;
            else
                pos_end = y - 1;              
            end
            
            pos_start = pos_end - Wr + 1; % Start pos of concave
            [~, I] = max(k(pos_start:pos_end,x,2));
            pos_max = pos_start + I - 1;
            Scr = k(pos_max,x,2)*Wr;
            %V(pos_max,x,2) = V(pos_max,x,2) + Scr;
            Vt(pos_max,x) = Vt(pos_max,x) + Scr;
            Wr = 0;
        end
    end
end

% Direction: \
bla = k(:,:,3) > 0;
for start=1:(img_w+img_h-1)
    % Initial values
    if (start <= img_w)
        x = start;
        y = 1;
    else
        x = 1;
        y = start - img_w + 1;        
    end
    done = false;
    
    while ~done
        if(bla(y,x))
            Wr = Wr + 1;
        end
        
        if ( Wr > 0 && (y == img_h || x == img_w || ~bla(y,x)) )
            if (y == img_h || x == img_w)
                % Reached edge of image
                pos_x_end = x;
                pos_y_end = y;
            else
                pos_x_end = x - 1;              
                pos_y_end = y - 1;
            end
            pos_x_start = pos_x_end - Wr + 1;
            pos_y_start = pos_y_end - Wr + 1;
            
            %d = diag(k(pos_y_start:pos_y_end, pos_x_start:pos_x_end, 3));
            % More efficient implementation than diag(..)
            d = k(((pos_x_start-1)*img_h + pos_y_start + 2*img_w*img_h):(img_h + 1):((pos_x_end-1)*img_h + pos_y_end + 2*img_w*img_h));
            [~, I] = max(d);
            pos_x_max = pos_x_start + I - 1;
            pos_y_max = pos_y_start + I - 1;
            Scr = k(pos_y_max,pos_x_max,3)*Wr;
            %V(pos_y_max,pos_x_max,3) = V(pos_y_max,pos_x_max,3) + Scr;
            Vt(pos_y_max,pos_x_max) = Vt(pos_y_max,pos_x_max) + Scr;
            Wr = 0;
        end
        
        if((x == img_w) || (y == img_h))
            done = true;
        else
            x = x + 1;
            y = y + 1;
        end
    end
end

% Direction: /
bla = k(:,:,4) > 0;
for start=1:(img_w+img_h-1)
    % Initial values
    if (start <= img_w)
        x = start;
        y = img_h;
    else
        x = 1;
        y = img_w+img_h-start;        
    end
    done = false;
    
    while ~done
        if(bla(y,x))
            Wr = Wr + 1;
        end
        
        if ( Wr > 0 && (y == 1 || x == img_w || ~bla(y,x)) )
            if (y == 1 || x == img_w)
                % Reached edge of image
                pos_x_end = x;
                pos_y_end = y;
            else
                pos_x_end = x - 1;              
                pos_y_end = y + 1;
            end
            pos_x_start = pos_x_end - Wr + 1;
            pos_y_start = pos_y_end + Wr - 1;
            
            %d = diag(flipud(k(pos_y_end:pos_y_start, pos_x_start:pos_x_end, 4))); 
            % More efficient implementation than diag(flipud(..))
            d = k(((pos_x_start-1)*img_h + pos_y_start + 3*img_w*img_h):(img_h - 1):((pos_x_end-1)*img_h + pos_y_end + 3*img_w*img_h));
            [~, I] = max(d);
            pos_x_max = pos_x_start + I - 1;
            pos_y_max = pos_y_start - I + 1;
            Scr = k(pos_y_max,pos_x_max,4)*Wr;
            %V(pos_y_max,pos_x_max,4) = V(pos_y_max,pos_x_max,4) + Scr;
            Vt(pos_y_max,pos_x_max) = Vt(pos_y_max,pos_x_max) + Scr;
            Wr = 0;
        end
        
        if((x == img_w) || (y == 1))
            done = true;
        else
            x = x + 1;
            y = y - 1;
        end
    end
end

%Vt = V(:,:,1) + V(:,:,2) + V(:,:,3) + V(:,:,4);

%% Connection of vein centres
Cd = zeros(img_h, img_w, 4);
for x=3:img_w-3
    for y=3:img_h-3
        Cd(y,x,1) = min(max(Vt(y,x+1),  Vt(y,x+2))  ,...
            max(Vt(y,x-1),  Vt(y,x-2)));   % Hor
        Cd(y,x,2) = min(max(Vt(y+1,x),  Vt(y+2,x))  ,...
            max(Vt(y-1,x),  Vt(y-2,x)));   % Vert         
        Cd(y,x,3) = min(max(Vt(y-1,x-1),Vt(y-2,x-2)),...
            max(Vt(y+1,x+1),Vt(y+2,x+2))); % \
        Cd(y,x,4) = min(max(Vt(y+1,x-1),Vt(y+2,x-2)),...
            max(Vt(y-1,x+1),Vt(y-2,x+2))); % /     
    end
end

veins = max(Cd,[],3);

% %% Plot results
% figure('Name', 'Second order derivatives');
% subplot(2,2,1);
%   imshow(fxx, []);
%   title('Horizontal');
% subplot(2,2,2);
%   imshow(fyy, []);
%   title('Vertical');
% subplot(2,2,3);
%   imshow(f11, []);
%   title('\');
% subplot(2,2,4);
%   imshow(f22, []);
%   title('/');
% 
% figure('Name', 'Curvatures');
% subplot(2,2,1);
%   %imshow(log(k(:,:,1) + 1), []);
%   imshow(k(:,:,1) > 0, []);
%   title('Horizontal');
% subplot(2,2,2);
%   %imshow(log(k(:,:,2) + 1), []);
%   imshow(k(:,:,2) > 0, []);
%   title('Vertical');
% subplot(2,2,3);
%   %imshow(log(k(:,:,3) + 1), []);
%   imshow(k(:,:,3) > 0, []);
%   title('\');
% subplot(2,2,4);
%   %imshow(log(k(:,:,4) + 1), []);
%   imshow(k(:,:,4) > 0, []);
%   title('/');
%   
% figure('Name', 'Scores');
% subplot(2,2,1);
%   imshow(V(:,:,1));
%   title('Horizontal');
% subplot(2,2,2);
%   imshow(V(:,:,2));
%   title('Vertical');
% subplot(2,2,3);
%   imshow(V(:,:,3));
%   title('\');
% subplot(2,2,4);
%   imshow(V(:,:,3));
%   title('/');
