#pragma once
#include <iostream>
#include <string>

using namespace std;

class JobCorrector {
private:
    // Helper function to find the minimum of three numbers
    int min3(int a, int b, int c) {
        int m = a;
        if (b < m) m = b;
        if (c < m) m = c;
        return m;
    }

public:
    // Classic DP Algorithm: Levenshtein (Edit) Distance
    int calculateDistance(string s1, string s2) {
        int len1 = s1.length();
        int len2 = s2.length();

        // 2D Array for DP table (Max 50x50 size for textbook simplicity)
        int dp[50][50];

        for (int i = 0; i <= len1; i++) {
            for (int j = 0; j <= len2; j++) {
                // If first string is empty, insert all characters of second string
                if (i == 0) {
                    dp[i][j] = j; 
                } 
                // If second string is empty, remove all characters of first string
                else if (j == 0) {
                    dp[i][j] = i; 
                } 
                // If characters are the same, no operation needed
                else if (s1[i - 1] == s2[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1]; 
                } 
                // If different, consider all 3 operations and take the minimum
                else {
                    dp[i][j] = 1 + min3(
                        dp[i][j - 1],    // Insert
                        dp[i - 1][j],    // Remove
                        dp[i - 1][j - 1] // Replace
                    );
                }
            }
        }
        return dp[len1][len2];
    }

    // Auto-Suggest Feature
    string suggestCorrection(string typoQuery, string activeJobs[], int totalJobs) {
        int minDistance = 999;
        string bestMatch = "";

        // Compare the typo against all known active jobs
        for (int i = 0; i < totalJobs; i++) {
            int dist = calculateDistance(typoQuery, activeJobs[i]);
            
            if (dist < minDistance) {
                minDistance = dist;
                bestMatch = activeJobs[i];
            }
        }

        // If the typo is only 1 or 2 letters off, suggest it!
        if (minDistance <= 2) {
            return bestMatch;
        } else {
            return "No close match found";
        }
    }
};