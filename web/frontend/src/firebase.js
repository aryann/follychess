import { initializeApp } from "firebase/app";
import { getFirestore } from "firebase/firestore";

// These values are public identifiers, not secrets; access control lives in
// the Firestore security rules. The apiKey is filled in from the Firebase
// console (Project settings > General > Your apps) via .env.local.
const firebaseConfig = {
  apiKey: import.meta.env.VITE_FIREBASE_API_KEY,
  authDomain: "follychess.firebaseapp.com",
  projectId: "follychess",
};

export const db = getFirestore(initializeApp(firebaseConfig));
