#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;


typedef struct Dir{
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;

/*functia de alocare memorie pentru capul de lista de tip File */
File * alocHeadFile (Dir * parent, char * name) {
	File * file = (File*)malloc(sizeof(File));
	if (!file)
		return NULL; //verificam alocare

	file->parent = parent;
	file->next = NULL;
	file->name = (char*)malloc(MAX_INPUT_LINE_SIZE * sizeof(char));
	if (!file->name) {
		free(file);
		return NULL;
	}
	strcpy(file->name, name);

	return file;
}

/*functia de alocare memorie pentru capul de lista tip Dir*/
Dir * alocHeadDir(Dir * parent, char * name) {
	Dir * dir = (Dir*)malloc(sizeof(Dir));
	if (!dir) //verificam alocarea structurii
		return NULL;
	dir->parent = parent;
	dir->next = NULL;
	dir->head_children_dirs = NULL;
	dir->head_children_files = NULL;
	dir->name = (char*)malloc(MAX_INPUT_LINE_SIZE * sizeof(char));
	if (!dir->name) {
		free(dir);
		return NULL;
	}
	strcpy(dir->name, name);

	return dir;
}

void distrList (Dir * parent) {
	File * temp = parent->head_children_files;
	File * urm;
	while (temp != NULL) {
		urm = temp->next;
		//printf("sterge structura cu name %s\n", temp->name);
		free(temp->name);
		free(temp);
		temp = urm;
	}
	
	parent->head_children_files = NULL;
	//free(parent);
}

void touch (Dir* parent, char* name) {
	if (parent->head_children_files == NULL) {
		parent->head_children_files = alocHeadFile(parent, name);
		return;
	} //cazul in care lista de fisiere este goala
	
	if (strcmp(parent->head_children_files->name, name) == 0) {
		printf("File already exists\n"); //cazul pt primul element din lista
		return;
	}

	File * temp = parent->head_children_files; //variabila cu care parcurgem lista
	
	while (temp->next != NULL) {
		if (strcmp (temp->name, name) == 0) {
			printf("File already exists\n");
			return;
		}
		temp = temp->next; //avansam in lista
	}
	temp->next = alocHeadFile(parent, name); //adaugam la final
}

void mkdir (Dir* parent, char* name) {
	if (parent->head_children_dirs == NULL) {
		parent->head_children_dirs = alocHeadDir(parent, name);
		return;
	} //cazul in care lista de directoare este goala

	if (strcmp(parent->head_children_dirs->name, name) == 0) {
		printf("Directory already exists\n");
		return; //cazul pentru primul element din lista de dirs
	}
	
	Dir * temp = parent->head_children_dirs; //variabila cu care parcurgem lista

	while (temp->next != NULL) {
		if (strcmp (temp->name, name) == 0) {
			printf("Directory already exists\n");
			return;
		}
		temp = temp->next; //avansam in lista
	}
	temp->next = alocHeadDir(parent, name); //adaugam la final
}

void ls (Dir* parent) {
	Dir * temp_dir = parent->head_children_dirs; //pozitionam capurile de liste
	File * temp_file = parent->head_children_files;

	while (temp_dir != NULL) {
		printf("%s\n", temp_dir->name); //afisam si parcurgem
		temp_dir = temp_dir->next;
	}

	while (temp_file != NULL) {
		printf("%s\n", temp_file->name); //afisam si parcurgem
		temp_file = temp_file->next;
	}
}

void rm (Dir* parent, char* name) {
	if (parent->head_children_files == NULL) {
		printf("Could not find the file\n");
		return;
	} //cazul in care lista de fisiere este goala

	File * temp = parent->head_children_files;
	File * prev = NULL;

	if (temp != NULL && strcmp(temp->name, name) == 0) {
		parent->head_children_files = temp->next;
		free(temp->name);
		free (temp);
		return; //cazul in care primul element este de eliminat
	}

	while (temp != NULL && strcmp(temp->name, name) != 0) {
		prev = temp;
		temp = temp->next; //parcurgem lista
	}
	if (temp == NULL) {
		printf("Could not find the file\n");
		return;
	}
	prev->next = temp->next; //eliminam elementul
	free(temp->name);
	free (temp);
}

void rmAll (Dir * parent) {
	if(parent == NULL)
		return; //conditia stop
	
	rmAll(parent->next); //apel recursiv
	distrList(parent); //distrugem lista
	free(parent->name);
	rmAll(parent->head_children_dirs);
	free(parent);
}

void rmdir (Dir* parent, char* name) {
	if (parent->head_children_dirs == NULL) {
		printf("Could not find the dir\n");
		return;
	} //cazul in care lista de directoare este goala

	Dir * temp = parent->head_children_dirs;
	Dir * prev = NULL;

	while (temp != NULL && strcmp(temp->name, name) != 0) {
		prev = temp;
		temp = temp->next; //parcurgem lista
	}

	if (temp == NULL) {
		printf("Could not find the dir\n"); //nu s-a gasit fisierul
		return;
	}

	if (prev == NULL) {
		parent->head_children_dirs = temp->next; //daca este primul element de eliminat
	}
	else 
		prev->next = temp->next;
	
	rmAll(temp->head_children_dirs); //stergem lista de directoare
	distrList(temp);
	free(temp->name);
	free(temp);

}
/*este aceeasi functie de rmdir, dar nu distrugem lista de directoare pentru a putea realiza mv-ul */

void rmdir_aux (Dir* parent, char* name) {
	if (parent->head_children_dirs == NULL) {
		printf("Could not find the dir\n");
		return;
	} 

	Dir * temp = parent->head_children_dirs;
	Dir * prev = NULL;

	while (temp != NULL && strcmp(temp->name, name) != 0) {
		prev = temp;
		temp = temp->next;
	}

	if (temp == NULL) {
		printf("Could not find the dir\n");
		return;
	}

	if (prev == NULL) {
		parent->head_children_dirs = temp->next;
	}
	else {
		prev->next = temp->next;
	}

	free(temp->name);
	free(temp);

}

void cd(Dir** target, char *name) {
	if (strcmp (name,  "..") == 0) {
		if ( (*target)->parent){
			(*target) = (*target)->parent; //ne pozitionam pe parent
			return;
		}
		else return;
	}

	Dir * temp = (*target)->head_children_dirs;
	while (temp != NULL && strcmp(temp->name, name) != 0) {
		temp = temp->next; //parcurgem lista
	}

	if (temp == NULL) {
		printf("No directories found!\n");
		return;
	}
	(*target) = temp; //schimbam pointerul pe elementul care ne intereseaza
}

char *pwd (Dir* target) {
	char * sir;
	if (target->parent == NULL){
		sir = malloc(sizeof(char) *MAX_INPUT_LINE_SIZE);
		sir[0] = '/'; //delimitare intre directoare
		sir[1]=  '\0'; 
		strcat(sir, target->name); //concatenam numele
		return sir;
	}

	sir = pwd(target->parent); //apel recursiv
	strcat(sir, "/");
	strcat(sir, target->name);
	return sir;
}

void stop (Dir* target) {
	rmAll(target);
	exit(0); //iese din program
}

void tree (Dir* target, int level) {
	if (target == NULL)
		return;
	Dir * temp_d = target->head_children_dirs; //pozitionam capetele de lista
	File * temp_f = target->head_children_files;
	for (; temp_d != NULL; temp_d = temp_d->next) {
		for (int i = 0; i < level; i ++){ //parcurgem lista de directoare
			printf("    ");
		}
		printf("%s\n", temp_d->name); //afisam
		tree(temp_d, level + 1); //apel recursiv la functie
	
	}
	for (; temp_f != NULL; temp_f = temp_f->next) { //parcurgem lista de fisiere

		for (int i = 0; i < level; i ++){
						printf("    ");
		}
		printf("%s\n", temp_f->name); //afisam
	}
}

void mv(Dir* parent, char *oldname, char *newname) {
	Dir * temp_d = parent->head_children_dirs; //pozitionam capurile de lista
	File * temp_f = parent->head_children_files;
	while (temp_d != NULL) {
		if (strcmp(temp_d->name, newname) == 0 ) {
			printf("File/Director already exists\n"); //cazul in care numele nou exista deja
			return;
		}
		temp_d = temp_d->next; //parcurgem
	}
	while (temp_f != NULL) {
		if (strcmp(temp_f->name, newname) == 0 ) {
			printf("File/Director already exists\n"); //cazul in care numele nou exista deja
			return;
		}
		temp_f = temp_f->next; //parcugem
	}

	temp_d = parent->head_children_dirs; //mv pentru directoare
	while (temp_d != NULL) {
		if (strcmp(temp_d->name, oldname) == 0 ) { //cautam numele vechi
			break;
		}
		temp_d = temp_d->next;
	}
	
	if (temp_d != NULL) {

		File * list = temp_d->head_children_files; //memoram lista de fisiere si directoare
		Dir * list_d = temp_d->head_children_dirs;
		rmdir_aux(parent, oldname); //stergem si cream
		mkdir(parent, newname);
		Dir * temp_list = parent->head_children_dirs;
		while (temp_list->next != NULL){ //la final de lista de directoare, vom lega listele memorate
			temp_list = temp_list->next;
		}
		temp_list->head_children_files = list;
		temp_list->head_children_dirs = list_d;
	}

	
	temp_f = parent->head_children_files; //mv pentru fisiere
	while (temp_f != NULL) {
		if (strcmp(temp_f->name, oldname) == 0 ) { //cautam numele vechi
			break;
		}
		temp_f = temp_f->next;
	}
	if(temp_f != NULL) {
		rm(parent, oldname); //stergem si cream
		touch(parent, newname);
	}

	if (temp_f == NULL && temp_d == NULL) {
		printf("File/Director not found\n"); //cazul in care nu exista fisiere cu numele vechi
		return;
	}
	
}

int main () {
	char * line, * function, * name, * newname;
	line = malloc(MAX_INPUT_LINE_SIZE * sizeof(char));
	if (!line)
		return -1; //exit code pentru eroare alocare
	Dir * homeDir = alocHeadDir(NULL, "home");
	Dir * currentDir = homeDir;
	size_t length;
	
	do {
		fgets (line, MAX_INPUT_LINE_SIZE, stdin);
		length = strlen(line);
		if (line[length - 1] == '\n')
			line[length - 1] = '\0';
		function = strtok(line, " ");
		if (strcmp(function, "touch") == 0) {
			name = strtok(NULL, "");
			touch(currentDir, name);
		}
		if (strcmp(function, "mkdir") == 0) {
			name = strtok(NULL, "");
			mkdir(currentDir, name);
		}
		if (strcmp(function, "ls") == 0) {
			ls(currentDir);
		}
		if (strcmp(function, "rm") == 0) {
			name = strtok(NULL, "");
			rm(currentDir, name);
		}
		if (strcmp(function, "rmdir") == 0) {
			name = strtok(NULL, "");
			rmdir(currentDir, name);
		}
		if (strcmp(function, "cd") == 0) {
			name = strtok(NULL, "");
			cd (&currentDir, name);
		}
		if (strcmp (function, "tree") == 0) {
			tree(currentDir, 0);
		}
		if (strcmp(function, "pwd") == 0) {
			char * sir = pwd(currentDir);
			printf("%s\n", sir);
			free(sir);
		}
		if (strcmp(function, "mv") == 0) {
			name = strtok(NULL, " ");
			newname = strtok(NULL, "");
			mv(currentDir, name, newname);
		}
		if (strcmp(function, "stop") == 0) {
			free(line);
			stop(homeDir);
		}
		
	} while (true);
	
	return 0;
}
