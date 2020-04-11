%% Lectura serial proyecto 1

% Lee constantemente valores del puerto serial.
% El PIC manda primero un valor entre 0 y 4 como indicador para saber
% que valor corresponde a cada variable y asi almacenarla

% Variables globales
global s x

% Reinicia los puertos. Sin esto se puede producir un error de que
% el puerto no se cerró correctamente en una ejecucion previa.
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

% Creamos nuestro objeto serial
% Puerto: COM11
% Baudrate: 9,600
% Timeout: 1 segundo (tiempo de lectura)
s = serial('COM11','BaudRate',9600,'Timeout',1);

% Abrimos la comunicacion
fopen(s);

% Lectura infinita de datos del puerto serial
while(1)
    
    % Lectura de la variable indicadora. 1 dato unicamente
    x = fread(s,1);
    disp('i = '); % Imprimimos en consola
    disp(x);

    switch (x)
        case 0
            celda = fread(s,1);
            disp('Celda: ');
            disp(celda);

        case 1
            luz = fread(s,1);
            disp('luz: ');
            disp(luz);

        case 2
            dist = fread(s,1);
            disp('Dist: ');
            disp(dist);

        case 3
            humo = fread(s,1);
            disp('Humo: ');
            disp(humo);

        case 4
            temp = fread(s,1);
            disp('Tempe: ');
            disp(temp);

    end
end

fclose(s);
