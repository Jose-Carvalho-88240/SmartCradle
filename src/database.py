def send_temp_hum(a,b):
  import pyrebase

  config = {
  "apiKey": "3bkkqHY9qniMHWG5w9Ck3yhrAJUh5Q4bUp3H9JA6",
  "authDomain": "projeto-gamma.firebaseapp.com",
  "databaseURL": "https://projeto-gamma.firebaseio.com",
  "storageBucket": "projeto-gamma.appspot.com"
  }

  firebase = pyrebase.initialize_app(config)
  db = firebase.database()


  data = {
    "humidity": float("{:.2f}".format(b)),
    "temperature": float("{:.2f}".format(a))
  }
  db.child("users").child("1XdXWXC26aNQ5TrEIX1z2p5ipiH2").child("parameters").update(data)

  return 1


def get_swing_flag():
  import pyrebase

  config = {
  "apiKey": "3bkkqHY9qniMHWG5w9Ck3yhrAJUh5Q4bUp3H9JA6",
  "authDomain": "projeto-gamma.firebaseapp.com",
  "databaseURL": "https://projeto-gamma.firebaseio.com",
  "storageBucket": "projeto-gamma.appspot.com"
  }

  firebase = pyrebase.initialize_app(config)
  db = firebase.database()
  ret = db.child("users").child("1XdXWXC26aNQ5TrEIX1z2p5ipiH2").child("parameters").get()

  x = int(ret.val()["swing_flag"])

  if(x == None): 
    return -1

  return x  

def get_live_flag():
  import pyrebase

  config = {
  "apiKey": "3bkkqHY9qniMHWG5w9Ck3yhrAJUh5Q4bUp3H9JA6",
  "authDomain": "projeto-gamma.firebaseapp.com",
  "databaseURL": "https://projeto-gamma.firebaseio.com",
  "storageBucket": "projeto-gamma.appspot.com"
  }

  firebase = pyrebase.initialize_app(config)
  db = firebase.database()
  ret = db.child("users").child("1XdXWXC26aNQ5TrEIX1z2p5ipiH2").child("parameters").get()

  x = int(ret.val()["live_flag"])

  if(x == None): 
    return -1
    
  return x    

def send_notification_flag(b):
  import pyrebase

  config = {
  "apiKey": "3bkkqHY9qniMHWG5w9Ck3yhrAJUh5Q4bUp3H9JA6",
  "authDomain": "projeto-gamma.firebaseapp.com",
  "databaseURL": "https://projeto-gamma.firebaseio.com",
  "storageBucket": "projeto-gamma.appspot.com"
  }

  firebase = pyrebase.initialize_app(config)
  db = firebase.database()


  data = {
    "notification_flag": int(b) 
  }
  db.child("users").child("1XdXWXC26aNQ5TrEIX1z2p5ipiH2").child("parameters").update(data)

  return 1

def send_swing_flag(b):
  import pyrebase

  config = {
  "apiKey": "3bkkqHY9qniMHWG5w9Ck3yhrAJUh5Q4bUp3H9JA6",
  "authDomain": "projeto-gamma.firebaseapp.com",
  "databaseURL": "https://projeto-gamma.firebaseio.com",
  "storageBucket": "projeto-gamma.appspot.com"
  }

  firebase = pyrebase.initialize_app(config)
  db = firebase.database()


  data = {
    "swing_flag": int(b) 
  }
  db.child("users").child("1XdXWXC26aNQ5TrEIX1z2p5ipiH2").child("parameters").update(data)

  return 1

#def get_acc_id(b):  probably will not be used
  import pyrebase

  config = {
  "apiKey": "3bkkqHY9qniMHWG5w9Ck3yhrAJUh5Q4bUp3H9JA6",
  "authDomain": "projeto-gamma.firebaseapp.com",
  "databaseURL": "https://projeto-gamma.firebaseio.com",
  "storageBucket": "projeto-gamma.appspot.com"
  }

  firebase = pyrebase.initialize_app(config)
  db = firebase.database()

  acc_ids = db.get()
  x = 0
  for child in acc_ids.each():
    #print(int(acc_ids.val()["prod_id"]))
    if (int(b) == int(child.val()["user_info"]["prod_id"])):
      return x
    x = x+1
    
  return -1  