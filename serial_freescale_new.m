function varargout = serial_freescale_new(varargin)
% SERIAL_FREESCALE_NEW M-file for serial_freescale_new.fig
%      SERIAL_FREESCALE_NEW, by itself, creates a new SERIAL_FREESCALE_NEW or raises the existing
%      singleton*.
%
%      H = SERIAL_FREESCALE_NEW returns the handle to a new SERIAL_FREESCALE_NEW or the handle to
%      the existing singleton*.
%
%      SERIAL_FREESCALE_NEW('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SERIAL_FREESCALE_NEW.M with the given input arguments.
%
%      SERIAL_FREESCALE_NEW('Property','Datas',...) creates a new SERIAL_FREESCALE_NEW or raises the
%      existing singleton*.  Starting from the left, property datas pairs are
%      applied to the GUI before serial_freescale_new_OpeningFcn gets called.  An
%      unrecognized property name or invalid datas makes property application
%      stop.  All inputs are passed to serial_freescale_new_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help serial_freescale_new

% Last Modified by GUIDE v2.5 23-Jan-2011 12:39:40

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @serial_freescale_new_OpeningFcn, ...
                   'gui_OutputFcn',  @serial_freescale_new_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before serial_freescale_new is made visible.
function serial_freescale_new_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to serial_freescale_new (see VARARGIN)

% Choose default command line output for serial_freescale_new
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes serial_freescale_new wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = serial_freescale_new_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in Open_Serial.
function Open_Serial_Callback(hObject, eventdata, handles)
% hObject    handle to Open_Serial (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%function Open_Serial_Callback(hObject, eventdata, handles)
global s;%����ȫ�ֱ���s,���ڱ�ʶ����
global rate;%����ȫ�ֱ���rate,���ڱ�ʾ����ͨ�ŵĲ�����
global COM;%����ȫ�ֱ���COM�����ڱ�ʶѡȡ��COM��
global save_flag;%�����־
global wait_flag;
global resume_flag;
save_flag=0;
wait_flag=0;
resume_flag=0;

%ͨ��Select_COM�������ؼ��ı�ȫ�ֱ���COM��ֵ������COM��ֵѡȡ�򿪵�COM��
if(COM==1)
s=serial('COM1','Parity','none','DataBits',8,'StopBits',1);
elseif(COM==2)
s=serial('COM2','Parity','none','DataBits',8,'StopBits',1);
elseif(COM==3)
s=serial('COM3','Parity','none','DataBits',8,'StopBits',1');
elseif(COM==4)
s=serial('COM4','Parity','none','DataBits',8,'StopBits',1);
elseif(COM==5)
s=serial('COM5','Parity','none','DataBits',8,'StopBits',1);
elseif(COM==6)
s=serial('COM6','Parity','none','DataBits',8,'StopBits',1);
elseif(COM==7)
s=serial('COM7','Parity','none','DataBits',8,'StopBits',1);
end
%ͨ��Baude�������ı�ȫ�ֱ���rate��ֵ������rate��ֵѡȡ��������Ӧ�Ĳ�����
if(rate==1)
set(s,'BaudRate',1200);
elseif(rate==2)
set(s,'BaudRate',2400);
elseif(rate==3)
set(s,'BaudRate',4800);
elseif(rate==4)
set(s,'BaudRate',9600);
elseif(rate==5)
set(s,'BaudRate',14400);
elseif(rate==6)
set(s,'BaudRate',19200);
elseif(rate==7)
set(s,'BaudRate',38400);
elseif(rate==8)
set(s,'BaudRate',57600);
elseif(rate==9)
set(s,'BaudRate',115200);
end

global ComQuerySize;
ComQuerySize=1024;
s=serial('COM1','Parity','none','DataBits',8,'StopBits',1);
s.BaudRate=9600;
s.InputBufferSize=131072;
s.Timeout=30;
s.ReadAsyncMode='continuous'
s.BytesAvailableFcnMode='byte';%�жϴ����¼�Ϊ��bytes-available Event��
s.BytesAvailableFcnCount=ComQuerySize;%���ջ�����ÿ�յ�n ���ֽ�ʱ�������ص�����
s.BytesAvailableFcn=@my_callback;
set(s,'BytesAvailableFcn',{@my_callback,handles});
fopen(s);%�򿪴���
a=num2str(datestr(now,'HH:MM:SS'));%���ʵʱʱ�䲢ת��Ϊ�ַ��ʹ���a������
set(handles.Start_Time,'String',a);%���ַ���a��ʾ�ھ�̬�洢Start��
guidata(hObject, handles); % ���½ṹ��






% --- Executes on button press in Clear.


% --- Executes on button press in Save.
function Save_Callback(hObject, eventdata, handles)
% hObject    handle to Save (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global save_flag;
save_flag=1;

% --- Executes on button press in Off_Serial.
function Off_Serial_Callback(hObject, eventdata, handles)
% hObject    handle to Off_Serial (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global s;
fclose(s);%�رմ����豸
delete(s);%ɾ����������
clear s;

% --- Executes on button press in Quit.
function Quit_Callback(hObject, eventdata, handles)
% hObject    handle to Quit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
clear
close all %�ر�GUI���洰��
clc

% --- Executes on selection change in Select_COM.
function Select_COM_Callback(hObject, eventdata, handles)
% hObject    handle to Select_COM (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns Select_COM contents as cell array
%        contents{get(hObject,'Datas')} returns selected item from Select_COM
global COM;%����ȫ�ֱ���COM
COM=1;%COM����ʼֵΪ1
rate=1;%rate����ʼֵΪ1
val=get(hObject,'value');%��������value��ֵ����val
%ͨ��valֵѡ��COM��ֵ
switch val
case 1
COM=1;
case 2
COM=2;
case 3
COM=3;
case 4
COM=4;
case 5
COM=5;
case 6
COM=6;
case 7
COM=7;
end

% --- Executes during object creation, after setting all properties.
function Select_COM_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Select_COM (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in Baude.
function Baude_Callback(hObject, eventdata, handles)
% hObject    handle to Baude (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns Baude contents as cell array
%        contents{get(hObject,'Datas')} returns selected item from Baude
global rate;%����ȫ�ֱ���rate
val=get(hObject,'value');%��������valueֵ����val
%ͨ��val��ֵѡ��rate ��ֵ
switch val
case 1
rate=1;
case 2
rate=2;
case 3
rate=3;
case 4
rate=4;
case 5
rate=5;
case 6
rate=6;
case 7
rate=7;
case 8
rate=8;
case 9
rate=9;
end

% --- Executes during object creation, after setting all properties.
function Baude_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Baude (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end




function edit2_Callback(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit2 as text
%        str2double(get(hObject,'String')) returns contents of edit2 as a double


% --- Executes during object creation, after setting all properties.
function edit2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit3_Callback(hObject, eventdata, handles)
% hObject    handle to Datas (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of Datas as text
%        str2double(get(hObject,'String')) returns contents of Datas as a double


% --- Executes during object creation, after setting all properties.
function edit3_CreateFcn(hObject, ~, ~)
% hObject    handle to Datas (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function Start_Time_Callback(hObject, eventdata, handles)
% hObject    handle to Start_Time (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of Start_Time as text
%        str2double(get(hObject,'String')) returns contents of Start_Time as a double


% --- Executes during object creation, after setting all properties.
function Start_Time_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Start_Time (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function my_callback(obj,event,handles)
global ComQuerySize;
global save_flag;
global wait_flag; %�ȴ�
global resume_flag;
[out,a]=fread(obj,ComQuerySize,'uint8');
out=out';    %ת��֮��Ϊ1��n�е�����ʽ
sample=zeros(1,512);   %�Բ������ݽ��д���
for j=1:512
    sample(1,j)=out(1,2*j-1)*256+out(1,2*j);
end
sample=sample/65536;   %��һ��  
set(handles.axes1,'Ylim',[0,1]);   %�趨y�᷶Χ
plot(handles.axes1,sample(1:512)); 
if(save_flag==1)
dlmwrite('sample.txt',sample,'\t');%��datas��������txt��ʽ�洢��a.txt��
end


    



% --- Executes during object deletion, before destroying properties.
function axes1_DeleteFcn(hObject, eventdata, handles)
% hObject    handle to axes1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes during object creation, after setting all properties.
function axes1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to axes1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: place code in OpeningFcn to populate axes1
