//分割文件，使用progressbar展示进度,通常实现方式
class MainForm:public Form            //存在问题 依赖倒置原则，抽象依赖于实现细节，采用进度条实现，后期需要修改
{                                     //展示方法，修改为数字显示等不便修改，控制和UI没有分离
    TextBox* txtFilePath;
    TextBox* txtFileNumber;
    ProgressBar *progressBar; //具体通知控件
public:
    void Button1_Click(){
        string filePath = txtFilePath->getText();
        int number = atoi(txtFileNumber->getText().c_str());
        FileSplitter splitter(filePath,number,progressBar);
        splitter.split();
    }
};
class FileSplitter
{
    string m_filePath;
    int m_fileNumber;
    ProgressBar* m_progressBar;
public:
    FileSplitter(const string& filePath,int fileNumber,ProgressBar *progressBar):
        m_filePath(filePath),m_fileNumber(fileNumber),m_progressBar(progressBar){

        }
    void split(){
        //1.读取大文件

        //2.分批次向小文件写入
        for(int i=0;i<m_fileNumber;i++){
            if(m_progressBar != nullptr){
                 m_progressBar->setValue((i+1)/m_fileNumber);
            }
        }
    }
};

//通知者保存订阅者对象的接口指针，维护订阅者的接口指针队列（用IProgress指针指向子类对象，就是实现了这个接口的类），
//每当有事件改变，遍历接口指针的队列，每个指针会调用接口中的抽象方法
//所有实现了这个接口的类，每个类分别调用自己的实现方式（基类指针指向子类，多态）
class IProgress{
    virtual void DoProgress(float value)=0;
    virtual ~IProgress(){}
};
class MainForm:public Form,public IProgress //一个父类一个接口 控制不依赖界面
{
    TextBox* txtFilePath;
    TextBox* txtFileNumber;
    ProgressBar *progressBar; //实现方式
public:
    void Button1_Click(){
        ConsoleNotifier cn;
        string filePath = txtFilePath->getText();
        int number = atoi(txtFileNumber->getText().c_str());
        FileSplitter splitter(filePath,number);
        splitter.add_IProgress(this);
        splitter.add_IProgress(&cn);
        splitter.split();
        splitter.remove_IProgress(this);
    }
    virtual void DoProgress(float value){
        progressBar->setValue(value);
    }
};
class FileSplitter
{
    string m_filePath;
    int m_fileNumber;
    vector<IProgress*> m_IProgressVector; //支持多个观察者
public:
    FileSplitter(const string& filePath,int fileNumber):
        m_filePath(filePath),m_fileNumber(fileNumber){

        }
    void add_IProgress(IProgress* iprogress){
        m_IProgressVector.push_back(iprogress);
    }
    void remove_IProgress(IProgress* iprogress){
        m_IProgressVector.remove(iprogress);
    }
    void split(){
        //1.读取大文件

        //2.分批次向小文件写入
        for(int i=0;i<m_fileNumber;i++){
            float progressValue = m_fileNumber;
            progressValue = (i+1)/m_fileNumber;
            onProgress(progressValue);
        }
    }
protected:
    void onProgress(float value){
        Vector<IProgress*>::Iterator itor = m_iprogress.begin();
        while(itor != m_iprogress.end()){
            *itor->DoProgress(value);
            itor++;
        }
    }
};
