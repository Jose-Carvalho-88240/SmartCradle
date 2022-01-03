#include "../inc/database.h"

//includes move to HEADER file

PyObject *pName, *pModule, *pDict;
PyObject *pFuncSend_notification_flag, *pFuncGet_live_flag, *pFuncGet_swing_flag, *pFuncSend_temp_hum, *pResult;


int send_temp_hum( float temp, float hum)
{
    //Call the function with arguments -> i = one integer , "ii" = two integers , "d" -> double mkvalue-style format string
    pResult = PyObject_CallFunction(pFuncSend_temp_hum, "ff", temp, hum);
    PyErr_Print();

    //Cleanup
    Py_XDECREF(pResult);

}

int get_swing_flag()
{
    //Call the function with arguments -> i = one integer , "ii" = two integers , "d" -> double mkvalue-style format string
    pResult = PyObject_CallFunction(pFuncGet_swing_flag, NULL);
    PyErr_Print();

    //Truncates the result to int since the Python function results
    //Translates from Python to C double
    int aux = (int)PyLong_AsLong(pResult);

    if (aux < 0)
    {
        //Cleanup
        Py_XDECREF(pResult);
        printf("ERROR GETTING FLAG");
        return -ERROR;
    }

    //Cleanup
    Py_XDECREF(pResult);

    return aux;
}

int get_live_flag()
{
    //Call the function with arguments -> i = one integer , "ii" = two integers , "d" -> double mkvalue-style format string
    pResult = PyObject_CallFunction(pFuncGet_live_flag, NULL);
    PyErr_Print();

    //Truncates the result to int since the Python function results
    //Translates from Python to C double
    int aux = (int)PyLong_AsLong(pResult);

     if (aux < 0)
    {
        //Cleanup
        Py_XDECREF(pResult);
        printf("ERROR GETTING FLAG");
        return -ERROR;
    }

    //Cleanup
    Py_XDECREF(pResult);

    //Py_FinalizeEx();
    return aux;
}

int send_notification_flag( int notification_flag)
{
    //Call the function with arguments -> i = one integer , "ii" = two integers , "d" -> double mkvalue-style format string
    pResult = PyObject_CallFunction(pFuncSend_notification_flag, "i", notification_flag);
    PyErr_Print();

    //Cleanup
    Py_XDECREF(pResult);
}

int initDatabase()
{
    pName = PyUnicode_FromString((char*)"database");

    //Import the Python Script
    pModule = PyImport_Import(pName);

    if(pModule == NULL)
    {
        printf("No module with that name");
        return -ERROR;
    }

    //Get the dictionary referent to the imported module
    pDict = PyModule_GetDict(pModule);

    Py_DECREF(pName);
    Py_DECREF(pModule);

    pFuncSend_notification_flag = PyDict_GetItemString(pDict, (char*)"send_notification_flag");
    
    //Check if its callable
    if (!PyCallable_Check(pFuncSend_notification_flag))
    {
        PyErr_Print();
    }

    pFuncSend_temp_hum = PyDict_GetItemString(pDict, (char*) "send_temp_hum");

    //Check if its callable
    if (!PyCallable_Check(pFuncSend_temp_hum))
    {
        PyErr_Print();
    }

    pFuncGet_live_flag = PyDict_GetItemString(pDict, (char*) "get_live_flag");

    //Check if its callable
    if (!PyCallable_Check(pFuncGet_live_flag))
    {
        PyErr_Print();
    }

    pFuncGet_swing_flag = PyDict_GetItemString(pDict, (char*) "get_swing_flag");
    
    //Check if its callable
    if (!PyCallable_Check(pFuncGet_swing_flag))
    {
        PyErr_Print();
    }
}

