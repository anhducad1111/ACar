import { StatusBar } from "expo-status-bar";
import React, { useState, useEffect } from "react";
import {
  StyleSheet,
  Text,
  View,
  Pressable,
  Alert,
  Animated,
  Easing,
} from "react-native";
import app from "./firebaseConfig";
import { getDatabase, ref, set, onValue, get } from "firebase/database";
import Icon from "react-native-vector-icons/FontAwesome5";

export default function App() {
  const database = getDatabase(app);
  const avoidedRef = ref(database, "solanduoi15/");
  const speedRef = ref(database, "speed/");
  const [count, setCount] = useState([]);
  const [speed, setSpeed] = useState([]);
  const spinValue = new Animated.Value(0);
  const spin = spinValue.interpolate({
    inputRange: [0, 1],
    outputRange: ["0deg", "360deg"],
  });

  useEffect(() => {
    onValue(avoidedRef, (snapshot) => {
      const countsData = snapshot.val();
      const counts = countsData.data;
      setCount(counts);
    });
  }, []);
  useEffect(() => {
    onValue(speedRef, (snapshot) => {
      const speedsData = snapshot.val();
      const speeds = speedsData.data;
      setSpeed(speeds);
    });
  }, []);

  const resetCount = async () => {
    try {
      await set(ref(database, "solanduoi15/"), {
        data: 0,
        reset: true,
      });
      console.log("Reset!");
      Alert.alert("Have been reset!");
      spinAnimation();
    } catch (error) {
      console.log(
        "Something went wrong with adding post to Realtime Database.",
        error
      );
      Alert.alert(
        "Error",
        "Something went wrong with reset. Please try again later."
      );
    }
  };
  
  const scaleSpeed = async (action) => {
    try {
      const snapshot = await get(ref(database, "speed/"));
      const currentData = snapshot.val();
      let newData;
      const currentNumber = currentData.data;
  
      if (action === "increase") {
        newData = currentNumber + 10;
      } else if (action === "decrease") {
        newData = Math.max(100, currentNumber - 10);
      } else {
        console.log("Invalid action");
        return;
      }
  
      await set(ref(database, "speed/"), {
        data: newData, 
      });
  
    } catch (error) {
      console.log(
        `Something went wrong with ${action}ing speed in Realtime Database.`,
        error
      );
      Alert.alert(
        "Error",
        `Something went wrong with ${action}ing speed. Please try again later.`
      );
    }
  };
  

  const spinAnimation = () => {
    spinValue.setValue(0);
    Animated.timing(spinValue, {
      toValue: 1,
      duration: 500,
      easing: Easing.linear,
      useNativeDriver: true,
    }).start();
  };

  return (
    <View style={styles.container}>
      <Text style={styles.text2}>Speed: {speed}</Text>
      <Text style={styles.text1}>
        How many times have you avoided obstacles?
      </Text>
      <Text style={styles.text2}>Count: {count}</Text>
      <Pressable style={styles.icon} onPress={resetCount}>
        <Animated.View style={{ transform: [{ rotate: spin }] }}>
          <Icon name="redo" color="#000" size={25} />
        </Animated.View>
      </Pressable>
      <Pressable style={styles.icon} onPress={() => scaleSpeed("increase")}>
          <Icon name="plus" color="#000" size={25} />
      </Pressable>
      <Pressable style={styles.icon} onPress={() => scaleSpeed("decrease")}>
          <Icon name="minus" color="#000" size={25} />
      </Pressable>
      <StatusBar style="auto" />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#fff",
    alignItems: "center",
    justifyContent: "center",
  },
  text1: {
    fontSize: 20,
    fontWeight: "bold",
  },
  text2: {
    fontSize: 18,
  },
  icon: {
    marginTop: 100,
  },
});