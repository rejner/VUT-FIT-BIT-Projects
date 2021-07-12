from scipy.io import wavfile
from scipy import stats
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import spectrogram

# sem stačí zadat jméno analyzovaného souboru (bez přípony)
file = "si2013"

fs, data = wavfile.read("..\\sentences\\" + file + '.wav')
fsq1, dataq1 = wavfile.read("..\\queries\\q1.wav")
fsq2, dataq2 = wavfile.read("..\\queries\\q2.wav")

time = np.arange(data.size) / fs

# normalizace hodnot z <int.min, int.max> na <-1,1>
data = data / 2**15
dataq1 = dataq1 / 2**15
dataq2 = dataq2 / 2**15

# ustředníme data, od každého vzorku odečteme průměrnou hodnotu
signal_length = len(data) # délka signálu (počet vzorků)
for sample in data:
    sample = sample - np.mean(data)

# graf 1
plt.figure(figsize=(9, 3))
plt.plot(time, data)
plt.gca().set_xlabel('t')
plt.gca().set_ylabel('signal')
plt.gca().set_title('"agency" and "screwdriver" vs. ' + file)
plt.tight_layout()
plt.savefig('..\\signal-' + file + '.png') # save fig

# vypočítáme spektrogram t = počet segmentů,
f_data, t_data, sgr = spectrogram(data, fs, "hamming", 400, 240, 511)
f_dataq1, t_dataq1, sgrq1 = spectrogram(dataq1, fsq1, "hamming", 400, 240, 511)
f_dataq2, t_dataq2, sgrq2 = spectrogram(dataq2, fsq2, "hamming", 400, 240, 511)
P = sgr                 # matice se spektrogramem
# F = AP

# matice nul 16 x 256
A = np.zeros((16, 256))

# naplním matici A
for row in range(0, 16):
    print_ones = False
    printed = 0

    for col in range(0, 256):
        if col == 16 * row:
            print_ones = True

        if print_ones:
            A[row][col] = 1
            printed += 1
            if printed == 16:
                print_ones = False

# VERIFIED

f_F, t_F, sgrF = spectrogram(data, fs, "hamming", 4000, 2400, 5111)
F_log = 10 * np.log10(sgrF+1e-20)


# prevod na PSD
sgr_log = 10 * np.log10(sgr+1e-20)
sgrq1_log = 10 * np.log10(sgrq1+1e-20)
sgrq2_log = 10 * np.log10(sgrq2+1e-20)

# test - vypočítat kouficienty z logaritmického spektra
F = np.matmul(A, sgr_log)
Q1 = np.matmul(A, sgrq1_log)
Q2 = np.matmul(A, sgrq2_log)

result1 = []
j = 0
index = 0

# QUERRY 1
# VÝPOČET PRAVDĚPODOBNOSTI ZAČÁTKU QUERRY VE VĚTĚ V ZÁVISLOSTI NA ČASE
# každý index obsahuje parametry věty, kde rozdíl mezi parametry je asi 1ms
while index <= t_data.size - 1:

    # sem se ukládají dočasné hodnoty, které se následně průměrují
    #  (výsledek je přesnější, počítáme pravděpodobnost v každé ms)
    temp_res = []

    # j značí index s parametry v querry, každá část věty se
    # porovnává s prvními 10 parametry querry (tedy začátek querry)
    while j <= 10:
        TMP_MATRIX_F = F[:, [index]]
        TMP_MATRIX_Q1 = Q1[:, [j]]
        AF = np.squeeze(np.asarray(TMP_MATRIX_F))
        AQ1 = np.squeeze(np.asarray(TMP_MATRIX_Q1))
        r, p_value = stats.pearsonr(AQ1, AF)

        # budeme brát v potaz pouze "statisticaly significant" hodnoty
        r = abs(r)
        temp_res.append(r)
        j += 1
        index += 1

        # brání v přetečení indexu a zásahu mimo matici
        if index + 1 >= t_data.size:
            break

    # reset indexu querry
    j = 0

    # zprůměrujeme naměřené hodnoty pro jeden úsek
    tmp = np.mean(temp_res)
    result1.append(tmp)


result2 = []
j = 0
index = 0
# QUERRY 2
# VÝPOČET PRAVDĚPODOBNOSTI ZAČÁTKU QUERRY VE VĚTĚ V ZÁVISLOSTI NA ČASE
# každý index obsahuje parametry věty, kde rozdíl mezi parametry je asi 1ms
while index <= t_data.size - 1:

    # sem se ukládají dočasné hodnoty, které se následně průměrují
    #  (výsledek je přesnější, počítáme pravděpodobnost v každé ms)
    temp_res = []

    # j značí index s parametry v querry, každá část věty se
    # porovnává s prvními 10 parametry querry (tedy začátek querry)
    while j <= 10:
        TMP_MATRIX_F = F[:, [index]]
        TMP_MATRIX_Q2 = Q2[:, [j]]
        AF = np.squeeze(np.asarray(TMP_MATRIX_F))
        AQ2 = np.squeeze(np.asarray(TMP_MATRIX_Q2))
        # hodnoty skóre počítáme pomocí pearsona
        r, p_value = stats.pearsonr(AQ2, AF)

        # budeme brát v potaz pouze "statisticaly significant" hodnoty
        r = abs(r)
        temp_res.append(r)
        j += 1
        index += 1

        # brání v přetečení indexu a zásahu mimo matici
        if index + 1 >= t_data.size:
            break

    j = 0 # reset indexu querry

    # mohlo se stát, že hodnoty byly statisticky bezvýznamné, výsledek budeme považovat
    # jako hodnotu 0
    if len(temp_res) == 0:
        temp_res.append(0.0)

    # zprůměrujeme naměřené hodnoty pro jeden úsek
    tmp = np.mean(temp_res)
    result2.append(tmp)

# graf score
plt.figure(figsize=(9, 3))
plt.plot(np.arange(len(result1)) / 10, result1, label='"agency"')
plt.plot(np.arange(len(result2)) / 10, result2, label='"screwdriver"')
plt.gca().set_xlabel('t')
plt.gca().set_ylabel('scores')
plt.tight_layout()
plt.legend()
plt.savefig('..\\score-' + file + '.png') # save fig


# vykreslíme spektrogram
plt.figure(figsize=(9, 3))
plt.pcolormesh(t_F, f_F, F_log)
plt.gca().set_xlabel('t')
plt.gca().set_ylabel('features')
cbar = plt.colorbar()
cbar.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)

plt.tight_layout()
plt.savefig('..\\spectogram-features' + file + '.png')



