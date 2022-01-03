#include "../inc/database.h"

//includes move to HEADER file

PyObject *pName, *pModule, *pDict;
PyObject *pFunc, *pResult;


int send_temp_hum( float temp, float hum)
{
    //pFunc is a reference to the function in Python
    pFunc = PyDict_GetItemString(pDict, (char*)"send_temp_hum");

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

    return 1;
}

int get_swing_flag()
{
    //pFunc is a reference to the function in Python
    pFunc = PyDict_GetItemString(pDict, (char*)"get_swing_flag");

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

    return aux;
}

int get_live_flag()
{
    //pFunc is a reference to the function in Python
    pFunc = PyDict_GetItemString(pDict, (char*)"get_live_flag");

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

    return aux;
}

int send_notification_flag( int notification_flag)
{
    //pFunc is a reference to the function in Python
    pFunc = PyDict_GetItemString(pDict, (char*)"send_notification_flag");

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

    return 1;
}

void initDatabase()
{
       //Translate from a C string to Python Code
    pName = PyUnicode_FromString((char*)"database");

    //Import the Python Script
    pModule = PyImport_Import(pName);

    if(pModule == NULL)
    {
        printf("No module with that name");
    }

    //Get the dictionary referent to the imported module
    pDict = PyModule_GetDict(pModule);

    //Cleanup
    Py_DECREF(pName);
    Py_DECREF(pModule);
}

