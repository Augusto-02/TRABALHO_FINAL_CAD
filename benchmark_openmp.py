import pandas as pd
import matplotlib.pyplot as plt
import os

# Caminho do CSV com os resultados
csv_path = "./"  # Coloque aqui o caminho completo se necessário

# Carregar o CSV
df = pd.read_csv(csv_path)

# Converter tamanho de array para string para uso nos nomes de arquivos
df["Array Size"] = df["Array Size"].astype(str)

# Criar diretório de saída dos gráficos
output_dir = "graficos_openmp"
os.makedirs(output_dir, exist_ok=True)

# Listas únicas para iteração
schedules = df["Schedule"].unique()
array_sizes = df["Array Size"].unique()

# Gerar gráficos
for schedule in schedules:
    for size in array_sizes:
        subset = df[(df["Schedule"] == schedule) & (df["Array Size"] == size)]
        subset = subset.sort_values("Threads")

        threads = subset["Threads"]
        tempo = subset["Time (s)"]
        speedup = subset["Speedup"]
        eficiencia = subset["Efficiency"]

        fig, axs = plt.subplots(1, 3, figsize=(18, 5))
        fig.suptitle(f"OpenMP - Schedule: {schedule} | Array: {size} elementos", fontsize=14)

        # Gráfico: Tempo
        axs[0].plot(threads, tempo, marker='o')
        axs[0].set_title("Tempo de Execução")
        axs[0].set_xlabel("Threads")
        axs[0].set_ylabel("Tempo (s)")
        axs[0].grid(True)

        # Gráfico: Speedup
        axs[1].plot(threads, speedup, marker='o', color='orange')
        axs[1].set_title("Speedup")
        axs[1].set_xlabel("Threads")
        axs[1].set_ylabel("Speedup")
        axs[1].grid(True)

        # Gráfico: Eficiência
        axs[2].plot(threads, eficiencia, marker='o', color='green')
        axs[2].set_title("Eficiência")
        axs[2].set_xlabel("Threads")
        axs[2].set_ylabel("Eficiência")
        axs[2].grid(True)

        # Ajustar layout e salvar imagem
        plt.tight_layout(rect=[0, 0.03, 1, 0.90])
        nome_arquivo = f"{output_dir}/grafico_{schedule}_{size}.png".replace(" ", "")
        plt.savefig(nome_arquivo)
        plt.close()
        print(f"Gráfico salvo em: {nome_arquivo}")
