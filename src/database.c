#include "../inc/database.h"
//#include </home/joao/buildroot-aulas-2021.02.6/output/host/include/python3.9/Python.h>
#include <python3.8/Python.h>  //change to the above one for compilling to the board


int send_temp_hum( float temp, float hum)
{
    PyObject *pName, *pModule, *pDict;
    PyObject *pFunc, *pResult;

    //Set PYTHONPATH to working directory
    setenv("PYTHONPATH",".",1);

    //Initialize the Python Interpreter
    Py_Initialize();

    //Translate from a C string to Python Code
    pName = PyUnicode_FromString((char*)"test");

    //Import the Python Script
    pModule = PyImport_Import(pName);

    if(pModule == NULL)
    {
        printf("No module with that name");
        return -ERROR;
    }

    //Get the dictionary referent to the imported module
    pDict = PyModule_GetDict(pModule);

    //Cleanup
    Py_DECREF(pName);
    Py_DECREF(pModule);

    //pFunc is a reference to the function in Python
    pFunc = PyDict_GetItemString(pDict, (char*)"multiply");

    //Cleanup
    Py_DECREF(pDict);

    //Check if its callable
    if (PyCallable_Check(pFunc))
    {   
        //Call the function with arguments -> i = one integer , "ii" = two integers , "d" -> double mkvalue-style format string
        pResult = PyObject_CallFunction(pFunc, "ff", temp, hum);
        PyErr_Print();
    }
    else
    {
        PyErr_BadArgument();
    }

    //Cleanup
    Py_DECREF(pFunc);
    Py_XDECREF(pResult);

    Py_FinalizeEx();
}

int get_swing_flag()
{
    PyObject *pName, *pModule, *pDict;
    PyObject *pFunc, *pResult;

    //Set PYTHONPATH to working directory
    setenv("PYTHONPATH",".",1);

    //Initialize the Python Interpreter
    Py_Initialize();

    //Translate from a C string to Python Code
    pName = PyUnicode_FromString((char*)"test");

    //Import the Python Script
    pModule = PyImport_Import(pName);

    if(pModule == NULL)
    {
        printf("No module with that name");
        return -ERROR;
    }

    //Get the dictionary referent to the imported module
    pDict = PyModule_GetDict(pModule);

    //Cleanup
    Py_DECREF(pName);
    Py_DECREF(pModule);

    //pFunc is a reference to the function in Python
    pFunc = PyDict_GetItemString(pDict, (char*)"get_swing_flag");

    //Cleanup
    Py_DECREF(pDict);

    //Check if its callable
    if (PyCallable_Check(pFunc))
    {
        //Call the function with no arguments
        //pResult = PyObject_CallFunction(pFunc, NULL);
        
        //Call the function with arguments -> i = one integer , "ii" = two integers , "d" -> double mkvalue-style format string
        pResult = PyObject_CallFunction(pFunc, NULL);
        PyErr_Print();
    }
    else
    {
        PyErr_Print();
    }

    //Truncates the result to int since the Python function results
    //Translates from Python to C double
    int aux = (int)PyLong_AsLong(pResult);

    if (aux < 0)
    {
        //Cleanup
        Py_DECREF(pFunc);
        Py_XDECREF(pResult);
        printf("ERROR GETTING FLAG");
        return -ERROR;
    }

    //Cleanup
    Py_DECREF(pFunc);
    Py_XDECREF(pResult);

    Py_FinalizeEx();
    return aux;
}

int get_live_flag()
{
    PyObject *pName, *pModule, *pDict;
    PyObject *pFunc, *pResult;

    //Set PYTHONPATH to working directory
    setenv("PYTHONPATH",".",1);

    //Initialize the Python Interpreter
    Py_Initialize();

    //Translate from a C string to Python Code
    pName = PyUnicode_FromString((char*)"test");

    //Import the Python Script
    pModule = PyImport_Import(pName);

    if(pModule == NULL)
    {
        printf("No module with that name");
        return -ERROR;
    }

    //Get the dictionary referent to the imported module
    pDict = PyModule_GetDict(pModule);

    //Cleanup
    Py_DECREF(pName);
    Py_DECREF(pModule);

    //pFunc is a reference to the function in Python
    pFunc = PyDict_GetItemString(pDict, (char*)"get_live_flag");

    //Cleanup
    Py_DECREF(pDict);

    //Check if its callable
    if (PyCallable_Check(pFunc))
    {
        //Call the function with no arguments
        //pResult = PyObject_CallFunction(pFunc, NULL);
        
        //Call the function with arguments -> i = one integer , "ii" = two integers , "d" -> double mkvalue-style format string
        pResult = PyObject_CallFunction(pFunc, NULL);
        PyErr_Print();
    }
    else
    {
        PyErr_Print();
    }

    //Truncates the result to int since the Python function results
    //Translates from Python to C double
    int aux = (int)PyLong_AsLong(pResult);

     if (aux < 0)
    {
        //Cleanup
        Py_DECREF(pFunc);
        Py_XDECREF(pResult);
        printf("ERROR GETTING FLAG");
        return -ERROR;
    }

    //Cleanup
    Py_DECREF(pFunc);
    Py_XDECREF(pResult);

    Py_FinalizeEx();
    return aux;
}

int send_notification_flag( int notification_flag)
{
    PyObject *pName, *pModule, *pDict;
    PyObject *pFunc, *pResult;

    //Set PYTHONPATH to working directory
    setenv("PYTHONPATH",".",1);

    //Initialize the Python Interpreter
    Py_Initialize();

    //Translate from a C string to Python Code
    pName = PyUnicode_FromString((char*)"test");

    //Import the Python Script
    pModule = PyImport_Import(pName);

    if(pModule == NULL)
    {
        printf("No module with that name");
        return -ERROR;
    }

    //Get the dictionary referent to the imported module
    pDict = PyModule_GetDict(pModule);

    //Cleanup
    Py_DECREF(pName);
    Py_DECREF(pModule);

    //pFunc is a reference to the function in Python
    pFunc = PyDict_GetItemString(pDict, (char*)"send_notification_flag");

    //Cleanup
    Py_DECREF(pDict);

    //Check if its callable
    if (PyCallable_Check(pFunc))
    {
        //Call the function with arguments -> i = one integer , "ii" = two integers , "d" -> double mkvalue-style format string
        pResult = PyObject_CallFunction(pFunc, "i", notification_flag);
        PyErr_Print();
    }
    else
    {
        PyErr_Print();
    }

    //Cleanup
    Py_DECREF(pFunc);
    Py_XDECREF(pResult);

    Py_FinalizeEx();
}

