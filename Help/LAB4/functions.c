off_t getfilelength(int file_descriptor)
{
	struct stat buf;
	if (fstat(file_descriptor, &buf) == -1) error("Cannot fstat file");
	return buf.st_size;
}

void sethandler(void (*function)(int), int sig)
{
	struct sigaction sig_act;
	memset(&sig_act, 0x00, sizeof(struct sigaction));
	sig_act.sa_handler = function;
	if (sigaction(sig, &sig_act, NULL) == -1) error("Error setting signal handler");
}
void fillaiostructs(struct aiocb *aiocbs, char **buffer, int file_descriptor, int blocksize)
{
	for (int i = 0; i<BLOCKS; i++)
    {
		memset(&aiocbs[i], 0, sizeof(struct aiocb));
		aiocbs[i].aio_fildes = file_descriptor;
		aiocbs[i].aio_offset = 0;
		aiocbs[i].aio_nbytes = blocksize;
		aiocbs[i].aio_buf = (void *) buffer[i];
		aiocbs[i].aio_sigevent.sigev_notify = SIGEV_NONE;
	}
}
void readdata(struct aiocb *aiocbs, off_t offset)
{
	aiocbs->aio_offset = offset;
	if (aio_read(aiocbs) == -1) error("Cannot read");
}
void writedata(struct aiocb *aiocbs, off_t offset)
{
	aiocbs->aio_offset = offset;
	if (aio_write(aiocbs) == -1) error("Cannot write");
}
void syncdata(struct aiocb *aiocbs)
{
	suspend(aiocbs);
	if (aio_fsync(O_SYNC, aiocbs) == -1) error("Cannot sync\n");
	suspend(aiocbs);
}
void cleanup(char **buffers, int* file_descriptor, int file_descriptor_nr)
{
    struct aiocb aiocbp;
	/*if (działanie przerwane w trakcie wykonywania)
    {
        for(int i=0;i<file_descriptor_nr;i++)
        {
            if ((aio_cancel_res = aio_cancel(file_descriptor[i],NULL)) == -1) error("Cannot cancel async. I/O operations");
            if(aio_cancel_res == AIO_NOTCANCELED) 
            {
                co gdy nie zakonczone
            }
        }
    }*/
	for (int i = 0; i<BLOCKS; i++) free(buffers[i]);
    for(int i=0;i<file_descriptor_nr;i++)
    {
	    if (TEMP_FAILURE_RETRY(fsync(file_descriptor[i])) == -1) error("Error running fsync");
    }
}