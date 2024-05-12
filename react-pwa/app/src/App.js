import React, { useState, useEffect } from 'react';
import io from 'socket.io-client';
import 'bootstrap/dist/css/bootstrap.min.css';

function App() {
  const [isLoggedIn, setIsLoggedIn] = useState(false);
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [doorStatus, setDoorStatus] = useState('locked'); 

  useEffect(() => {
    const socket = io(process.env.REACT_APP_SOCKET_URL); 

    socket.on('door_status', (data) => {
      const status = data.data.split(',')[0]; // Assuming the status is the first part of the message
      setDoorStatus(status === 'unlock' ? 'unlocked' : 'locked');
    });

    return () => socket.disconnect();
  }, []);

  const handleLogin = async (e) => {
    e.preventDefault();

    try {
      const response = await fetch(`${process.env.REACT_APP_BACKEND_URL}/login`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ username, password }),
      });

      if (!response.ok) {
        throw new Error('Login failed');
      }

      const data = await response.json();
      localStorage.setItem('token', data.access_token); // Store the token
      setIsLoggedIn(true);
    } catch (error) {
      alert(error.message);
    }
  };

  const handleUnlock = async () => {
    // Get the current timestamp in ISO format
    const timestamp = new Date().toISOString();

    try {
        const response = await fetch(`${process.env.REACT_APP_BACKEND_URL}/open-door`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({ timestamp })
        });

        if (!response.ok) {
            throw new Error('Failed to unlock door');
        }

        const data = await response.json();
        alert(data.message); 
    } catch (error) {
        alert(error.message);
    }
  };

  const doorStatusStyle = {
    color: doorStatus === 'unlocked' ? 'green' : 'red',
  };

  return (
    <div className="container p-5">
      <div className="row justify-content-center">
        <div className="col-md-6">
          {isLoggedIn ? (
            <>
              <div className="text-center mt-5">
                <h2>Unlock door</h2>
                <button onClick={handleUnlock} className="btn btn-primary mt-3">Unlock Door</button>
              </div>
              <div className="text-center mt-3">
                <span style={doorStatusStyle}>Door status: {doorStatus}</span>
              </div>
            </>
          ) : (
            <div className="card p-4 mt-5">
              <form onSubmit={handleLogin}>
                <h2 className="text-center">Login</h2>
                <div className="mb-3">
                  <label htmlFor="username" className="form-label">Username</label>
                  <input 
                    type="text" 
                    className="form-control" 
                    id="username" 
                    value={username}
                    onChange={e => setUsername(e.target.value)}
                    placeholder="Username" 
                    required />
                </div>
                <div className="mb-3">
                  <label htmlFor="password" className="form-label">Password</label>
                  <input 
                    type="password" 
                    className="form-control" 
                    id="password" 
                    value={password}
                    onChange={e => setPassword(e.target.value)}
                    placeholder="Password" 
                    required />
                </div>
                <div className="text-center">
                  <button type="submit" className="btn btn-success">Login</button>
                </div>
              </form>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

export default App;