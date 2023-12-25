// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyAk_KLxh6MjPmra7t1n2VqkYufjQ4y3ABE",
  authDomain: "acar-9d140.firebaseapp.com",
  projectId: "acar-9d140",
  storageBucket: "acar-9d140.appspot.com",
  messagingSenderId: "960476677881",
  appId: "1:960476677881:web:cee0f75f65fce0a5b18428",
  databaseURL: "https://acar-9d140-default-rtdb.asia-southeast1.firebasedatabase.app/",
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

// Initialize Realtime Database and get a reference to the service
export default app;